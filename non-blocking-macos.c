// this is just for reference and this file will be deleted soon

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

SSL_CTX* create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // Initialize the OpenSSL library
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx) {
    // Set the key and cert for SSL_CTX
    // Use SSL_CTX_use_certificate_file() and SSL_CTX_use_PrivateKey_file()
    // to load the certificate and private key files
    if( SSL_CTX_use_certificate_file(ctx, "../certificates/tls.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if( SSL_CTX_use_PrivateKey_file(ctx, "../certificates/tls.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

typedef struct Connection {
    int conn_fd;
    SSL *ssl;
    struct Connection *next;
} Connection;

#define PORT 8080

int make_socket_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

#define BUFFER_SIZE 1024

void handle_request(Connection* c, int fd, struct kevent *event, int kq) {
    char buffer[BUFFER_SIZE];
    ssize_t count = SSL_read(c->ssl, buffer, sizeof(buffer)); // read(fd, buffer, BUFFER_SIZE - 1);
    buffer[count] = '\0'; // Null-terminate what we've read for easier handling

    printf("Request: %s\n", buffer);

    // Simplified check for keep-alive. Real implementation should parse request more thoroughly.
    int keep_alive = strstr(buffer, "Connection: keep-alive") != NULL;

    char response[1024];
    // Adjust response based on whether keep-alive is requested
    if (keep_alive) {
        char body[512];
        snprintf(body, sizeof(body), 
            "Hello, persinstent world. kq: %d, fd: %d", kq, fd);

        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: %lu\r\n\r\n%s", strlen(body), body);
    } else {
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nGoodbye, World!");
    }

    // write(fd, response, strlen(response));
    SSL_write(c->ssl, response, strlen(response));

    if (!keep_alive) {
        close(fd); // Close if not keep-alive
        // IMPORTANT: Remove from kqueue monitoring if closing
        EV_SET(event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(kq, event, 1, NULL, 0, NULL);
    }
    // If keep-alive, we do nothing special here - the fd remains open and monitored by kqueue
}

#define MAX_EVENTS 64

Connection* findConnection(Connection *conn, int fd) {
    Connection *c = conn;
    while( c != NULL ) {
        if( c->conn_fd == fd ) {
            return c;
        }
        c = c->next;
    }
    return NULL;
}

void printSSLError(int err) {
    printf("printSSLError: %d\n", err);
    switch (err) {
        case SSL_ERROR_WANT_READ:
            printf("SSL_ERROR_WANT_READ\n");
            // SSL_accept wants to read more data from the socket
            break;
        case SSL_ERROR_WANT_WRITE:
        printf("SSL_ERROR_WANT_WRITE\n");
            // SSL_accept wants to write more data to the socket
            break;
        case SSL_ERROR_SYSCALL:
            // Some I/O error occurred. The OpenSSL error queue may contain more information.
            if (errno != 0) {
                perror("SSL_accept (SSL_ERROR_SYSCALL)");
            } else {
                fprintf(stderr, "EOF observed that violates the protocol\n");
            }
            break;
        case SSL_ERROR_SSL:
            // A failure in the SSL library occurred, usually a protocol error.
            ERR_print_errors_fp(stderr); // Prints the error strings for all errors that OpenSSL has recorded, emptying the error queue.
            break;
        default:
            // Handle other errors
            break;
    }    
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

    SSL_CTX *ctx = create_context();
    printf("SSL context created\n");
    configure_context(ctx);
    printf("SSL context configured\n");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    make_socket_non_blocking(sockfd);

    int kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }

    struct kevent event;
    EV_SET(&event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        perror("kevent register");
        exit(EXIT_FAILURE);
    }

    Connection *conn = NULL;

    struct kevent events[MAX_EVENTS];

    while (1) {
        printf("Waiting for events... sockfd: %d\n", sockfd);
        int n = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (n == -1) {
            perror("kevent wait");
            continue;
        }

        for (int i = 0; i < n; i++) {
            if (events[i].flags & EV_ERROR) {
                // Error occurred; close socket and remove from kqueue
                perror("EV_ERROR");
                close(events[i].ident);
            } else if (events[i].flags & EV_EOF) { // Check for EOF
                printf("Connection closed by peer %lu\n", events[i].ident);
                close(events[i].ident); // Close the socket when connection is closed by peer
            } else if (events[i].ident == sockfd) {
                // New connection
                while (1) {
                    struct sockaddr_in cli_addr;
                    socklen_t cli_len = sizeof(cli_addr);
                    int conn_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
                    if (conn_fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            // Processed all incoming connections
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }
                    printf("Accepted new connection on fd %d\n", conn_fd);

                    make_socket_non_blocking(conn_fd);

                    //int enable = 1; // 10 seconds of idle before the first keepalive probe
                    //setsockopt(sockfd, IPPROTO_TCP, SO_KEEPALIVE, &enable, sizeof(enable));

                    EV_SET(&event, conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                        perror("kevent add conn_fd");
                        close(conn_fd);
                    }
                    printf("Added new connection to kqueue\n");
                    SSL *ssl = SSL_new(ctx); // `ctx` is your SSL context
                    if( ssl == NULL ) {
                        ERR_print_errors_fp(stderr);
                        exit(EXIT_FAILURE);
                    }
                    printf("SSL created\n");
                    if( SSL_set_fd(ssl, conn_fd) <= 0 ) {
                        ERR_print_errors_fp(stderr);
                        exit(EXIT_FAILURE);
                    }
                    printf("SSL set fd\n");
                    int counter = 100;
                    while(1) {
                        int r = SSL_accept(ssl);
                        if ( r != 1) {
                            printf("SSL_accept failed, r: %d.\n", r);
                            int erro = SSL_get_error(ssl, r);

                            if( erro == SSL_ERROR_WANT_READ || erro == SSL_ERROR_WANT_WRITE ) {
                                // just retry after 5 milliseconds
                                msleep(5);
                            }
                            
                            if( counter == 0) {
                                printf("Counter reached 0. Exiting.\n");
                                exit(EXIT_FAILURE);
                            }
                            //exit(EXIT_FAILURE);
                        } else {
                            break;
                        }
                        counter--;
                    }
                
                    printf("SSL connection established\n");
                    // SSL connection is established, use SSL_read and SSL_write to handle data
                    Connection *c = malloc(sizeof(Connection));
                    c->conn_fd = conn_fd;
                    c->ssl = ssl;
                    c->next = NULL;

                    if( conn == NULL ) {
                        conn = c;
                    } else {
                        c->next = conn;
                        conn = c;
                    }
                    
                }
            } else {
                printf("Handling request on fd %lu\n", events[i].ident);
                Connection* c = findConnection(conn, events[i].ident);
                // Existing connection has data
                handle_request(c, events[i].ident, &events[i], kq);
            }
        }
    }

    close(sockfd);
    return 0;
}