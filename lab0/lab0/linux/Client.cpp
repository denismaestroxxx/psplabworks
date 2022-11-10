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
using namespace std;
using namespace rapidjson;
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);
constexpr auto WORKERS_COUNT = 2;
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
map<int, double> results_map;
void inputMatrix(int** arr, int size);
void outputMatrix(int** arr, int size);
void outputResult(map<int, double>, int size);
void process(int id, poolItem worker, int** arr, int i, int j, int size);
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

int main() {
	for (int i = 0; i < WORKERS_COUNT; i++) {
		poolItem item = poolItem();
		item.isOpen = false;
		item.port = BASE_PORT + i;
		item.isEmpty = false;
		pool.push(item);
	}
	int size;
	std::cout << "Input matrix size" << endl;
	std::cin >> size;

	int** arr = new int* [size];
	int** algebraicComplements = new int* [size];
	for (int i = 0; i < size; i++)
		arr[i] = new int[size];
	inputMatrix(arr, size);
	/*size = 30;
	int** arr = new int* [30];
	arr[0] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[1] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[2] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[3] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[4] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[5] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[6] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[7] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[8] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[9] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[10] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[11] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[12] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[13] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[14] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[15] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[16] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[17] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[18] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[19] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[20] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[21] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[22] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[23] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[24] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[25] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[26] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[27] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[28] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};
	arr[29] = new int[30] { 1, 234, 4324, 234, 234, 234, 234, 234, 432, 432, 423, 532, 234, 23432, 234, 32432, 234, 4323, 5434, 324, 234, 32432, 324, 324, 324, 234, 234, 234, 342, 234};*/
	outputMatrix(arr, size);

	int id = 0;
	for (int i = 0;i < size;i++) {
		for (int j = 0; j < size; j++) {
			bool queued = false;
			while (!queued) {
				poolItem worker = pool.pop();
				if (!worker.isEmpty) {
					std::thread tA(process, id, worker, arr, i, j, size);
					tA.detach();
					queued = true;
					id = id + 1;
				}
			}
		}
	}

	int totalResults = size * size;
	while (results_map.size() != totalResults) {
		//sleep(1);
	}
	outputResult(results_map, size);
	for (int i = 0; i < WORKERS_COUNT; i++) {
		poolItem item = pool.pop();
		if (!item.isEmpty) {
			if (item.isOpen) {
				close(item.sock);
			}
		}
	}
	cout << "press any key to exit" << endl;
	int nothing;
	cin >> nothing;
	return 0;
}
void process(int id, poolItem worker, int** arr, int i, int j, int size)
{
int socket_desc;
socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	cout << "Connecting to server with port " << worker.port << endl;
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
	writer.Key("i");
	writer.Int(i);
	writer.Key("j");
	writer.Int(j);
	writer.Key("size");
	writer.Int(size - 1);
	writer.Key("array");
	writer.StartArray();
	for (int ki = 0; ki < size; ki++)
	{
		if (ki != i)
		{
			writer.StartArray();
			for (int kj = 0;kj < size;kj++) {
				if (kj != j)
				{
					writer.Int(arr[ki][kj]);
				}
			}
			writer.EndArray();
		}
	}
	writer.EndArray();
	writer.EndObject();
	header h = header();
	h.size = s.GetSize();
	cout << s.GetString() << endl;
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
	cout << "Given response json:" << buffer << endl;
	Document d;
	d.Parse(buffer);
	pool.push(worker);
	results_map[id] = d["algebraicComplement"].GetDouble();
	cout << "added response with id:" << id << endl;
}

void inputMatrix(int** arr, int size)
{
	int i, j;

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			std::cout << "input [" << i << "," << j << "]" << endl;
			std::cin >> arr[i][j];
		}
	}
}

void outputMatrix(int** arr, int size)
{
	int i, j;
	for (i = 0;i < size;i++)
	{
		for (j = 0;j < size;j++)
		{
			std::cout << arr[i][j] << " ";
		}
		std::cout << endl;
	}
	std::cout << endl;
}

void outputResult(map<int, double> results, int size)
{
	int i;
	int totalSize = size * size;
	int row = 0;
	map <int, double> ::iterator it = results.begin();
	for (int i = 0; it != results.end(); it++, i++) 
	{
		std::cout << it->second << " ";
		if (++row == size) {
			std::cout << endl;
			row = 0;
		}
	}
	std::cout << endl;
}
