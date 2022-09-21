#define _WINSOCK_DEPRECATED_NO_WARNINGS
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

int OutputValidation(char* buffer)
{
    int s = strlen(buffer);
    for (int i = 0; i < s; ++i)
    {
        char tmp = buffer[i];
        if (!isalpha(tmp) || strchr("AEIOUYaeiouy", tmp))
            return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // ����������� windows sockets
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    // ������� ��������� �����, �� ���� �������������� ���
    int serverSocket;
    struct sockaddr_in client_address;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) {
        // � ��� ������� ����������� - ������� �� �� �����
        wchar_t* s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, NULL);
        printf((char*)s);
    }

    // ������ ��������� ���������� ������ (���� �����������, ����, �������� ���������)
    memset((char*)&client_address, 0, sizeof(client_address));
    client_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(8080);

    // �������� ���������� �� ���������� ������
    connect(serverSocket, (sockaddr*)&client_address, sizeof(client_address));

    // ����������� ���
    send(serverSocket, "test", sizeof(char) * 4, 0);

    // ������ �� ������ ��������� �� �������
    char receiveBuffer[256];
    memset(receiveBuffer, 0, sizeof(char) * 256);

    recv(serverSocket, receiveBuffer, sizeof(char) * 256, 0);
    if (OutputValidation(receiveBuffer))
    {
        printf("Invalid output");
        return 1;
    }
    printf("Answer is: ");
    printf(receiveBuffer);

    return 0;
}
