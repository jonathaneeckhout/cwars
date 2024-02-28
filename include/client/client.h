#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

typedef struct
{
    int sockfd;
    bool connected;
} client_t;

client_t *client_init();
bool client_connect(client_t *client, const char *address, int port);
void client_cleanup(client_t **client);
void client_loop_once(client_t *client);

#endif // CLIENT_H
