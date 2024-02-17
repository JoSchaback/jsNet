#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <socket.h>

int main() {
    printf("Hello, World!\n");

    uint64_t socket_fd = jsNet_sever_socket(
        jsNet_IPV4, 
        jsNet_TCP, 
        "0.0.0.0", 
        8080);

    if( socket_fd < 0 ) {
        printf("Error creating socket\n");
    } else {
        printf("Socket created\n");
    }

    return 0;
}