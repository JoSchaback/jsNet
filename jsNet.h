
// include a few libraries that are cross-platform and
// are required in the lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Socket {
    int fileDescriptor;

} Socket;

void jsNet_Socket_create(Socket* socket, int port);
void jsNet_Socket_close(Socket* socket);

