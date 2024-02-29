#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#include "common/linked_list.h"

typedef struct
{
    int sockfd;
    bool connected;
    linked_list_t *out_message_queue;
    linked_list_t *in_message_queue;
} client_t;

client_t *client_init();
bool client_connect(client_t *client, const char *address, int port);
void client_cleanup(client_t **client);
void client_loop_once(client_t *client);

void client_send_ping(client_t *client);

#endif // CLIENT_H
