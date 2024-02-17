#include "jsNet.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


void jsNet_init() {
    printf("jsNet initialized\n");
}

int64_t jsNet_Socket_create(
    enum jsNet_AddressFamily addressFamily, 
    enum jsNet_SocketType socketType) 
{
    // TODO add conidiiton for addressFamily etc

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }
    return sockfd;
}
