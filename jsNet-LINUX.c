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

    
void jsNet_Socket_close(jsNet_Socket* socket) {

}


void jsNet_Socket_create(Socket* socket, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
}