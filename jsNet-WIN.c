#ifndef UNICODE
#define UNICODE 1
#endif

#include "jsNet.h"

// link with Ws2_32.lib

#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

WSADATA wsaData;

void jsNet_init() {
    // Declare and initialize variables
    //wsaData = {0};
    int iResult = 0;

//    int i = 1;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
    }
}

int64_t jsNet_Socket_create(
    enum jsNet_AddressFamily addressFamily, 
    enum jsNet_SocketType socketType) 
{ 
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
   
    return sockfd;
}
