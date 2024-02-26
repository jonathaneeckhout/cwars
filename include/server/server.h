#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct
{
    int sockfd;
    SSL_CTX *ctx;
    SSL *ssl;
} Server;

Server *server_init(const char *key_path, const char *cert_path);
void server_cleanup(Server **server);

#endif // SERVER_H
