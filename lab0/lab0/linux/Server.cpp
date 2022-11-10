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
#include "./common.h"
using namespace std;
using namespace rapidjson;
#include <cmath>
constexpr auto BASE_PORT = 5001;
constexpr auto SERVERADDR = "127.0.0.1";
constexpr auto HEADER_SIZE = sizeof(header);

int clientsCount = 0;
int receive(int sock, char* buffer, int chunk_size);
int process(int client_socket);
double algebraicComplement(int** arr, int i, int j, int size);
double determinant(int** arr, int size);

int main()
{
    char buff[1024];

    int serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Error socket" << endl;
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
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SO_ERROR) {
        cout << "Bind function failed with error" << endl;
        return -1;
    }

    if (listen(serverSocket, 0) == SO_ERROR) {
        cout << "Listen function failed with error" << endl;
        return -1;
    }

    printf("Waiting for connections...\n");

    int clientSocket;
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    while ((clientSocket = accept(serverSocket, NULL, NULL))) {
        clientsCount++;
        cout << "Connected client" << endl;
 
        std::thread tA(process, clientSocket);
        tA.detach();
    }

    return 0;
}

int process(int lpParam) {
    int client_socket;
    client_socket = lpParam;
    char headerBuffer[HEADER_SIZE] = { 0 };
    //char buffer[1024] = { 0 };
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
        int** arr = new int* [size];
        const Value& arrayValue = d["array"];
        for (int i = 0; i < size; i++) {
            arr[i] = new int[size];
            for (int j = 0; j < size; j++) {
                arr[i][j] = arrayValue[i][j].GetInt();
            }
        }
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("algebraicComplement");
        writer.Double(algebraicComplement(arr, d["i"].GetInt(), d["j"].GetInt(), size));
        writer.EndObject();
        header responseHeader = header();
        responseHeader.size = s.GetSize();
        char* header_data = new char[HEADER_SIZE];
        responseHeader.serialize(header_data);
        send(client_socket, header_data, HEADER_SIZE, 0);
        send(client_socket, s.GetString(), s.GetSize(), 0);
        cout << "Sent result:" << s.GetString() << endl;
    }
    printf("Disconnected\n");
    clientsCount--;
    return 0;
}

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

double algebraicComplement(int** arr, int i, int j, int size)
{
    return pow(-1., i + j) * determinant(arr, size);
}

double determinant(int** arr, int size)
{
    int i, j;
    double det = 0;       //переменная определителя
    int** matr;         //указатель
    if (size == 1)     // 1-е условие , размер 1
    {
        det = arr[0][0];
    }
    else if (size == 2)    // 2-е условие , размер 2
    {
        det = arr[0][0] * arr[1][1] - arr[0][1] * arr[1][0];    //
    }
    else
    {
        matr = new int* [size - 1]; //создание динамического массива
        for (i = 0;i < size;++i)
        {
            for (j = 0;j < size - 1;++j)
            {
                if (j < i)
                {
                    matr[j] = arr[j];
                }
                else
                    matr[j] = arr[j + 1];
            }
            det += pow(-1., (i + j)) * determinant(matr, size - 1) * arr[i][size - 1];    //подсчеты
        }
        delete[] matr;  //удаляем массив
    }
    return det; //возвращаем значение определителя
}
