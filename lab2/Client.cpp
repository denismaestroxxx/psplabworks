#define UA_ARCHITECTURE_POSIX
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/memorybuffer.h"
#include <stack> 
#include <thread>
#include <boost/thread.hpp>
#include <map>
#include "./common.h"
#include <chrono>
using namespace std;
using namespace rapidjson;
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);
constexpr auto WORKERS_COUNT = 4;
struct poolItem {
	int port;
	bool isOpen;
	int sock;
	bool isEmpty;
};
template <typename T> class ThreadSafeStack {
public:
	ThreadSafeStack() {
		m_stack = stack<poolItem>();
	}
	void push(const T& item) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_stack.push(item);
	}

	T pop() {
		boost::mutex::scoped_lock lock(m_mutex);
		if (!m_stack.empty()) {
			T item = m_stack.top();
			m_stack.pop();
			return item;
		}
                poolItem emptyItem;
                emptyItem.isEmpty = true;
		return emptyItem;
	}

private:
	mutable boost::mutex m_mutex;
	std::stack<T> m_stack;
};
ThreadSafeStack<poolItem> pool;

template <typename T, typename T2> class ThreadSafeMap {
public:
	ThreadSafeMap() {
		m_map = {};
	}

	std::map<T, T2> getAndclear() {
		boost::mutex::scoped_lock lock(m_mutex);
		std::map<T, T2> mapToReturn;
		mapToReturn.insert(m_map.begin(), m_map.end());
		m_map = {};
		return mapToReturn;
	}

	void push(T id, T2 value) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_map[id] = value;
	}

	std::map<T, T2> getMap() {
		return m_map;
	}

	int size() {
		boost::mutex::scoped_lock lock(m_mutex);
		return m_map.size();
	}

private:
	mutable boost::mutex m_mutex;
	std::map<T, T2> m_map;
};
ThreadSafeMap<int, double> safe_map;
map<int, double> results_map;
double getResult(map<int, double> results);
void process(int id, poolItem worker, double start, double end, int n);
int receive(int sock, char* buffer, int chunk_size)
{
	int offset = 0;

	while (chunk_size > 0)
	{
		int n = recv(sock, buffer + offset, chunk_size, 0);
		offset += n;
		chunk_size -= n;
	}
	return offset;
}
double calculateIntegral(double start, double end, double step, int n);

int main() {
	for (int i = 0; i < WORKERS_COUNT; i++) {
		poolItem item = poolItem();
		item.isOpen = false;
		item.port = BASE_PORT + i;
		pool.push(item);
	}
	double start;
	double end;
	int n;
	double eps;
	/*std::cout << "Input start" << endl;
	std::cin >> start;
	std::cout << "Input end" << endl;
	std::cin >> end;
	std::cout << "Input epsilon" << endl;
	std::cin >> eps;*/
	start = -1;
	end = 20;
	n = 1;
	eps = 0.001;
	double step = (end - start) / WORKERS_COUNT;
	auto start_time = std::chrono::steady_clock::now();
	double previousResult, currentResult;
	currentResult = calculateIntegral(start, end, step, n);
	double loss;
	do {
		previousResult = currentResult;
		n = 2 * n;
		currentResult = calculateIntegral(start, end, step, n);
		loss = fabs(previousResult - currentResult);
		std::cout << "Current loss " << loss << " Previous:" << previousResult << " Current:" << currentResult << endl;
	} while (loss > eps);

	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Total elapsed time:" << elapsed_ms.count() << " ms" << endl;
	std::cout << "Square:" << fabs(currentResult) << endl;
	for (int i = 0; i < WORKERS_COUNT; i++) {
		poolItem item = pool.pop();
		if (!item.isEmpty) {
			if (item.isOpen) {
				header h = header();
				h.disconnect = true;
				char* header_data = new char[HEADER_SIZE];
				h.serialize(header_data);
				send(item.sock, header_data, HEADER_SIZE, 0);
				close(item.sock);
			}
		}
	}
	cout << "press any key to exit" << endl;
	int nothing;
	cin >> nothing;
	return 0;
}

double calculateIntegral(double start, double end, double step, int n) {
	auto start_time = std::chrono::steady_clock::now();
	int id = 0;
	double workerStart;
	double workerEnd;
	for (int i = 0;i < WORKERS_COUNT;i++) {
		bool queued = false;
		while (!queued) {
			poolItem worker = pool.pop();
			if (!worker.isEmpty) {
				workerStart = start + step * i;
				workerEnd = start + step * (i + 1);
				std::thread tA(process, id, worker, workerStart, workerEnd, n);
				tA.detach();
				queued = true;
				id = id + 1;
			}
		}
	}
	while (safe_map.size() != WORKERS_COUNT) {
		//cout << "Map size" << results_map.size() << endl;
	}
	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	//std::cout << "Iteration elapsed time:" << elapsed_ms.count() << " ms" << endl;

	return getResult(safe_map.getAndclear());
}

void process(int id, poolItem worker, double start, double end, int n)
{
	int socket_desc;
        socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	//cout << "Connecting to server with port " << worker.port << endl;
	if (!worker.isOpen) {
		worker.sock = socket(AF_INET, SOCK_STREAM, 0);
		worker.isOpen = true;
		if (worker.sock < 0) {
			cout << "Socket error" << endl;
			return;
		}
		sockaddr_in dest_addr;
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(worker.port);
		dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		if (connect(worker.sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0)
		{
			cout << "Connection Failed" << endl;
			return;
		}
	}

	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("start");
	writer.Double(start);
	writer.Key("end");
	writer.Double(end);
	writer.Key("n");
	writer.Int(n);
	writer.EndObject();
	header h = header();
	h.size = s.GetSize();
	//cout << s.GetString() << endl;
	char* header_data = new char[HEADER_SIZE];
	h.serialize(header_data);
	send(worker.sock, header_data, HEADER_SIZE, 0);
	if (send(worker.sock, s.GetString(), s.GetSize(), 0) == SO_ERROR) {
		cout << "send failed" << endl;
	}

	char headerBuffer[HEADER_SIZE] = { 0 };
	receive(worker.sock, headerBuffer, HEADER_SIZE);
	header responseHeader;
	responseHeader.deserialize(headerBuffer);
	char* buffer = new char[responseHeader.size];
	buffer[responseHeader.size] = '\0';
	receive(worker.sock, buffer, responseHeader.size);
	//cout << "Given response json:" << buffer << endl;
	Document d;
	d.Parse(buffer);
	pool.push(worker);
	safe_map.push(id, d["result"].GetDouble());
	cout << "finished" << id << endl;
	//cout << "added response with id:" << id << endl;
}

double getResult(map<int, double> results)
{
	map <int, double> ::iterator it = results.begin();
	double result = 0.0;
	for (int i = 0; it != results.end(); it++, i++)
	{
		result = result + it->second;
	}

	return result;
}
