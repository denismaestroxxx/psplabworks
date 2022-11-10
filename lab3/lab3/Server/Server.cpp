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
#include "../Common/common.h"
#include "mergeSort.h"
using namespace std;
using namespace rapidjson;
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);

int clientsCount = 0;
int receive(SOCKET sock, char* buffer, int chunk_size);
DWORD WINAPI process(LPVOID client_socket);

int main()
{
    char buff[1024];
    if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
    {
        printf("Error WSAStartup %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error socket %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    cout << "Will listen on " << BASE_PORT << " port" << endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(BASE_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVERADDR);

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind function failed with error: " << WSAGetLastError() << endl;
        return -1;
    }

    if (listen(serverSocket, 0) == SOCKET_ERROR) {
        cout << "Listen function failed with error:" << WSAGetLastError() << endl;
        return -1;
    }

    cout << "Waiting for connections..." << endl;

    SOCKET clientSocket;
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    while ((clientSocket = accept(serverSocket, (sockaddr*)&client_addr, &client_addr_size))) {
        clientsCount++;
        cout << "Connected client" << endl;

        QueueUserWorkItem(process, &clientSocket, WT_EXECUTEDEFAULT);
    }

    return 0;
}

DWORD WINAPI process(LPVOID lpParam) {
    SOCKET client_socket;
    client_socket = ((SOCKET*)lpParam)[0];
    char headerBuffer[HEADER_SIZE] = { 0 };
    char header_data[HEADER_SIZE] = { 0 };
    header requestHeader;
    int bytes_received = 0;
    while (true) {
        bytes_received = receive(client_socket, headerBuffer, HEADER_SIZE);
        if (bytes_received == 0) {
            continue;
        }
        requestHeader.deserialize(headerBuffer);
        char* buffer = new char[requestHeader.size];
        buffer[requestHeader.size] = '\0';
        receive(client_socket, buffer, requestHeader.size);
        cout << "Given json:" << buffer << endl;
        Document d;
        d.Parse(buffer);
        int size = d["size"].GetInt();
        int* arr = new int[size];
        const Value& arrayValue = d["array"];
        for (int i = 0; i < size; i++) {
            arr[i] = arrayValue[i].GetInt();
        }
        MergeSort sorter = MergeSort();
        sorter.mergeSort(arr, 0, size-1);
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("sortedArray");
        writer.StartArray();
        for (int i = 0; i < size; i++)
        {
            writer.Int(arr[i]);
        }
        writer.EndArray();
        writer.EndObject();
        header responseHeader = header();
        responseHeader.size = s.GetSize();
        responseHeader.serialize(header_data);
        send(client_socket, header_data, HEADER_SIZE, 0);
        send(client_socket, s.GetString(), s.GetSize(), 0);
        cout << "Sent result:" << s.GetString() << endl;
        break;
    }
    closesocket(client_socket);
    cout << "Client disconnected";
    clientsCount--;
    return 0;
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
