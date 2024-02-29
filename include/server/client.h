#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include <netinet/in.h>

#include "common/linked_list.h"

typedef struct
{
    int id;
    int sockfd;
    struct sockaddr_in addr;
    bool connected;
    bool logged_in;
    linked_list_t *out_message_queue;
    linked_list_t *in_message_queue;
} client_t;

client_t *client_init(int sockfd, struct sockaddr_in addr);
void client_cleanup(client_t **client);

void client_loop_once(client_t *client);

#endif // CLIENT_H
