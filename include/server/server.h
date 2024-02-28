#ifndef SERVER_H
#define SERVER_H

#include "common/linked_list.h"

typedef struct
{
    int sockfd;
    linked_list_t *clients;
} Server;

Server *server_init(int port);
void server_cleanup(Server **server);
void server_loop_once(Server *server);

#endif // SERVER_H
