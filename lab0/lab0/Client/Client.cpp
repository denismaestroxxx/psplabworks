#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <winsock2.h> 
#pragma comment(lib, "WS2_32.lib")
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/memorybuffer.h"
#include <stack> 
#include <thread>
#include <boost/thread.hpp>
#include <map>
#include <chrono>
#include "../Common/common.h"
using namespace std;
using namespace rapidjson;
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);
constexpr auto WORKERS_COUNT = 5;
struct poolItem {
	int port;
	bool isOpen;
	SOCKET socket;
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

	T* pop() {
		boost::mutex::scoped_lock lock(m_mutex);
		if (!m_stack.empty()) {
			T item = m_stack.top();
			m_stack.pop();
			return &item;
		}

		return nullptr;
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
void inputMatrix(int** arr, int size);
void outputMatrix(int** arr, int size);
void outputResult(map<int, double>, int size);
void process(int id, poolItem worker, int** arr, int i, int j, int size);
int receive(SOCKET sock, char* buffer, int chunk_size);

int main() {
	for (int i = 0; i < WORKERS_COUNT; i++) {
		poolItem item = poolItem();
		item.isOpen = false;
		item.port = BASE_PORT + i;
		pool.push(item);
	}
	int size;
	/*std::cout << "Input matrix size" << endl;
	std::cin >> size;

	int** arr = new int* [size];
	int** algebraicComplements = new int* [size];
	for (int i = 0; i < size; i++)
		arr[i] = new int[size];
	inputMatrix(arr, size);*/
	size = 5;
int** arr = new int* [5];
arr[0] = new int[5]{ 11, 234, -543, 234, 234 };
arr[1] = new int[5]{ 134, 234, 432, 123, 234 };
arr[2] = new int[5]{ 1, 234, 4324, 234, 13 };
arr[3] = new int[5]{ 32, 234, 444, -234, 234 };
arr[4] = new int[5]{ 1, 234, 434, 234, 234 };
	//size = 10;
	//int** arr = new int* [10];
	//arr[0] = new int[10] { 11, 234, -543, 234, 234, 234, 234, 234, 24, 432};
	//arr[1] = new int[10] { 134, 234, 432, 123, 234, 234, 234, 234, 34, 432};
	//arr[2] = new int[10] { 1, 234, 4324, 234, 13, 12, 342, 41, 432, 432};
	//arr[3] = new int[10] { 32, 234, 444, -234, 234, 234, 13, 234, 432, 432};
	//arr[4] = new int[10] { 1, 234, 434, 234, 234, 234, 960, 742, 432, 432};
	//arr[5] = new int[10] { 1, 234, 434, 234, 234, 234, 234, 234, 432, 432};
	//arr[6] = new int[10] { 231, 234, 4324, 554, 234, 234, -234, 234, 432, 432};
	//arr[7] = new int[10] { 1, 234, 4324, 234, 234, -234, 644, 244, 432, 411};
	//arr[8] = new int[10] { 1, 234, -141, 234, 234, 234, 234, 234, 432, 432};
	//arr[9] = new int[10] { 213, 234, 4324, 234, 234, 234, 234, 234, 432, 432};
	outputMatrix(arr, size);

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData)) {
		printf("WSAStart error %d\n", WSAGetLastError());
		return 1;
	}
	auto start_time = std::chrono::steady_clock::now();
	int id = 0;
	for (int i = 0;i < size;i++) {
		for (int j = 0; j < size; j++) {
			bool queued = false;
			while (!queued) {
				poolItem* worker = pool.pop();
				if (worker != nullptr) {
					poolItem w = *worker;
					std::thread tA(process, id, w, arr, i, j, size);
					tA.detach();
					queued = true;
					id = id + 1;
				}
			}
		}
	}

	int totalResults = size * size;
	while (safe_map.size() != totalResults) {
		//Sleep(100);
	}

	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ns = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "elapsed time:" << elapsed_ns.count() << " ms" << endl;
	outputResult(safe_map.getAndclear(), size);
	for (int i = 0; i < WORKERS_COUNT; i++) {
		poolItem* item = pool.pop();
		if (item != nullptr) {
			poolItem worker = *item;
			if (worker.isOpen) {
				header h = header();
				h.disconnect = true;
				char* header_data = new char[HEADER_SIZE];
				h.serialize(header_data);
				send(worker.socket, header_data, HEADER_SIZE, 0);
				closesocket(worker.socket);
			}
		}
	}
	WSACleanup();
	cout << "press any key to exit" << endl;
	int nothing;
	cin >> nothing;
	return 0;
}
void process(int id, poolItem worker, int** arr, int i, int j, int size)
{
	cout << "Connecting to server with port " << worker.port << endl;
	if (!worker.isOpen) {
		worker.socket = socket(AF_INET, SOCK_STREAM, 0);
		worker.isOpen = true;
		if (worker.socket < 0) {
			cout << "Socket error " << WSAGetLastError() << endl;
			return;
		}
		sockaddr_in dest_addr;
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(worker.port);
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
		if (connect(worker.socket, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0)
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
	send(worker.socket, header_data, HEADER_SIZE, 0);
	if (send(worker.socket, s.GetString(), s.GetSize(), 0) == SOCKET_ERROR) {
		cout << "send failed with error: " << WSAGetLastError() << endl;
	}

	char headerBuffer[HEADER_SIZE] = { 0 };
	receive(worker.socket, headerBuffer, HEADER_SIZE);
	header responseHeader;
	responseHeader.deserialize(headerBuffer);
	char* buffer = new char[responseHeader.size];
	buffer[responseHeader.size] = '\0';
	receive(worker.socket, buffer, responseHeader.size);
	cout << "Given response json:" << buffer << endl;
	Document d;
	d.Parse(buffer);
	pool.push(worker);
	safe_map.push(id, d["algebraicComplement"].GetDouble());
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

int receive(SOCKET sock, char* buffer, int chunk_size)
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