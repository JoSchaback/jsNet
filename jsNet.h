
// include a few libraries that are cross-platform and
// are required in the lib
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for int64_t
#include <string.h>
#include <stdbool.h>

enum jsNet_SocketType {
    jsNet_TCP,
    jsNet_UDP
};

enum jsNet_AddressFamily {
    jsNet_IPV4,
    jsNet_IPV6
};


void jsNet_init();

int64_t jsNet_Socket_create(enum jsNet_AddressFamily addressFamily, enum jsNet_SocketType socketType);

