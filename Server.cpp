#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <winsock.h>
#include <socketapi.h>

#include <iostream>
#include <thread>

void MyFunc(char* buffer)
{
    int k = 0;
    int s = strlen(buffer);
    for (int i = 0; i < s; ++i)
    {
        char tmp = buffer[i];
        if (!strchr("AEIOUYaeiouy", tmp))
        {
            buffer[k] = tmp;
            ++k;
        }
    }
    for (; k < s; ++k)
    {
        buffer[k] = 0;
    }
}

int InputValidation(char* buffer)
{
    int s = strlen(buffer);
    for (int i = 0; i < s; ++i)
    {
        char tmp = buffer[i];
        if (!isalpha(tmp))
            return 1;
    }
    return 0;
}

void MyThread(int& readSocket)
{
    if (readSocket < 0) {
        printf("ERROR on accept");
    }

    // ініціалізуємо буфер для зчитки даних
    char buffer[256];
    memset(buffer, 0, sizeof(char) * 256);

    // читаємо дані із сокету
    int bytesRead = recv(readSocket, buffer, sizeof(char) * 256, 0);
    if (bytesRead < 0) {
        // обробляємо помилку читання
        wchar_t* s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, NULL);
        printf((char*)s);
    }

    if (InputValidation(buffer))
    {
        strcpy(buffer, "Invalid input\n");
        printf(buffer);
    }
    else
    {
        printf("Here is the message: %s\n", buffer);

        MyFunc(buffer);
    }

    send(readSocket, buffer, sizeof(char) * 256, 0);
    closesocket(readSocket);
}

int main(int argc, char* argv[])
{
    // Ініціалізація windows sockets
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        // вивести помилку ініціалізації
        wchar_t* s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, NULL);
        printf((char*)s);
    }

    // задаємо параметри серверного сокету (на якій адресі слухаємо, порт, сімейство протоколу)
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    // прив'язуємо адресу, протокол, порт до конкретного сокету
    if (bind(serverSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR on binding");
    }

    // конфігуруємо чергу очікування серверного сокету, 
    listen(serverSocket, 5);


    while (true)
    {
        // очікуємо на з'єдання та приймаємо його на інший сокет
        clilen = sizeof(cli_addr);
        int readSocket = accept(serverSocket, (struct sockaddr*)&cli_addr, &clilen);
        // ЗАВДАННЯ: винести виконання ітерації циклу після accept на окремий потік. Застосувати std::thread
        std::thread t(MyThread, std::ref(readSocket));
        t.join();
    }

    return 0;
}