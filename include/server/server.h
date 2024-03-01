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

void server_handle_input(Server *server);
void server_handle_output(Server *server);

#endif // SERVER_H
