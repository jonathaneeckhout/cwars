#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct
{
    int sockfd;
} Server;

Server *server_init(int port);
void server_cleanup(Server **server);
void server_loop_once(Server *server);

#endif // SERVER_H
