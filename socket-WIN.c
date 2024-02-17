#ifndef UNICODE
#define UNICODE 1
#endif

#include "socket.h"

// link with Ws2_32.lib

#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

WSADATA wsaData;
char isInitialized = 0;

int64_t jsNet_sever_socket(
    enum jsNet_AddressFamily addressFamily, 
    enum jsNet_SocketType socketType,
    const char* ip, 
    int port)
{
    if( !isInitialized ) {
        // Initialize Winsock
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            wprintf(L"WSAStartup failed: %d\n", iResult);
        }        
    }

    int iFamily = AF_UNSPEC;
    int iType = 0;
    int iProtocol = 0;

    iFamily   = AF_INET;
    iType     = SOCK_DGRAM;
    iProtocol = IPPROTO_TCP;    
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == INVALID_SOCKET) {
        perror("socket");
        return -1;
    }    

    struct sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = htonl(INADDR_ANY);
    serverService.sin_port = htons(27015);

    if (bind(sockfd, (SOCKADDR *) &serverService, sizeof(serverService)) == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }


    return sockfd;
}
