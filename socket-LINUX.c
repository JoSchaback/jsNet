#include "socket.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

int64_t jsNet_sever_socket(
    enum jsNet_AddressFamily addressFamily, 
    enum jsNet_SocketType socketType,
    const char* ip, 
    int port) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, ip, &server_address.sin_addr.s_addr) != 1) {
        fprintf(stderr, "Invalid IP address format\n");
        return -1;
    }

    int bind_status = bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (bind_status == -1) {
        perror("bind");
        return -1;
    }

    return sockfd;
}
