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
#include <chrono>
#include "../Common/common.h"
#include <random>
using namespace std;
using namespace rapidjson;
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);

void process(int* array, int size);
int receive(SOCKET sock, char* buffer, int chunk_size);
int main() {
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData)) {
		printf("WSAStart error %d\n", WSAGetLastError());
		return 1;
	}
	auto start_time = std::chrono::steady_clock::now();
	//int* array = new int[10] { 1, 4, 5, 1, 5, 3, 7, 2, 9, 10};
	//int size = 10;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 100000); // distribution in range

	int size = 500000;
	int* array = new int[size];
	for (int i = 0; i < size; i++) {
		array[i] = dist(rng);
	}
	process(array, size);
	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "elapsed time:" << elapsed_ms.count() << " ms" << endl;
	WSACleanup();
	cout << "press any key to exit" << endl;
	int nothing;
	cin >> nothing;
	return 0;
}

void process(int* array, int size)
{

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(BASE_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	if (connect(serverSocket, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0)
	{
		cout << "Connection Failed" << endl;
		return;
	}

	StringBuffer s;
	Writer<StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("size");
	writer.Int(size);
	writer.Key("array");
	writer.StartArray();
	for (int i = 0; i < size; i++)
	{
		writer.Int(array[i]);
	}
	writer.EndArray();
	writer.EndObject();
	header h = header();
	h.size = s.GetSize();
	//cout << s.GetString() << endl;
	char* header_data = new char[HEADER_SIZE];
	h.serialize(header_data);
	send(serverSocket, header_data, HEADER_SIZE, 0);
	if (send(serverSocket, s.GetString(), s.GetSize(), 0) == SOCKET_ERROR) {
		cout << "send failed with error: " << WSAGetLastError() << endl;
	}

	char headerBuffer[HEADER_SIZE] = { 0 };
	receive(serverSocket, headerBuffer, HEADER_SIZE);
	header responseHeader;
	responseHeader.deserialize(headerBuffer);
	char* buffer = new char[responseHeader.size];
	buffer[responseHeader.size] = '\0';
	receive(serverSocket, buffer, responseHeader.size);
	cout << "Given response:" << buffer << endl;
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