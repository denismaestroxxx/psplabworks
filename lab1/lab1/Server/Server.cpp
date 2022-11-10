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
using namespace std;
using namespace rapidjson;
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);

int clientsCount = 0;
int receive(SOCKET sock, char* buffer, int chunk_size);
DWORD WINAPI process(LPVOID client_socket);

typedef double(*pointFunc)(double);
double f(double x) {
    return pow(x, 3) * cos(x);
}

double integral(pointFunc f, double start, double end, int n) {
    double x, step;
    double sum = 0.0;
    double fx;
    step = (end - start) / n;

    for (int i = 0; i < n; i++) {
        x = start + i * step;
        fx = f(x);
        sum += fx;
    }
    return (sum * step);
}

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

    int serverNumber = 0;
    cout << "Input server number: " << endl;
    cin >> serverNumber;
    int serverPort = BASE_PORT + serverNumber;
    cout << "Will listen on " << serverPort << " port";

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(SERVERADDR);

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind function failed with error: " << WSAGetLastError() << endl;
        return -1;
    }

    if (listen(serverSocket, 0) == SOCKET_ERROR) {
        cout << "Listen function failed with error:" << WSAGetLastError() << endl;
        return -1;
    }

    printf("Waiting for connections...\n");

    SOCKET clientSocket;
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    while ((clientSocket = accept(serverSocket, (sockaddr*)&client_addr, &client_addr_size))) {
        clientsCount++;
        cout << "Connected client" << endl;

        DWORD tid;
        HANDLE t1 = CreateThread(NULL, 0, process, &clientSocket, 0, &tid); //Создание потока для получения данных
        if (t1 == NULL) {
            cout << "Thread Creation Error: " << WSAGetLastError() << endl;
        }
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
        requestHeader.deserialize(headerBuffer);
        cout << requestHeader.disconnect << endl;
        if (requestHeader.disconnect == true) {
            cout << "Client disconnected";
            break;
        }
        if (bytes_received == 0) {
            continue;
        }
        char* buffer = new char[requestHeader.size];
        buffer[requestHeader.size] = '\0';
        receive(client_socket, buffer, requestHeader.size);
        cout << "Given json:" << buffer << endl;
        Document d;
        d.Parse(buffer);
        double start = d["start"].GetDouble();
        double end = d["end"].GetDouble();
        int n = d["n"].GetInt();
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("result");
        writer.Double(integral(f, start, end, n));
        writer.EndObject();
        header responseHeader = header();
        responseHeader.size = s.GetSize();
        responseHeader.serialize(header_data);
        send(client_socket, header_data, HEADER_SIZE, 0);
        send(client_socket, s.GetString(), s.GetSize(), 0);
        cout << "Sent result:" << s.GetString() << endl;
        //delete buffer;
    }
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
