#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jsNet.h>

int main() {
    printf("Hello, World!\n");

    uint64_t socket_fd = jsNet_Socket_create(jsNet_IPV4, jsNet_TCP);

    if( socket_fd < 0 ) {
        printf("Error creating socket\n");
    } else {
        printf("Socket created\n");
    }

    return 0;
}