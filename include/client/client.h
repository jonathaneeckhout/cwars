#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

typedef struct
{
    int sockfd;
} client_t;

client_t *client_init();
bool client_connect(client_t *client, const char *address, int port);
void client_cleanup(client_t **client);

#endif // CLIENT_H
