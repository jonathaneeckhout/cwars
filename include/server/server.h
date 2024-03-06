#ifndef SERVER_H
#define SERVER_H

#include "common/linked_list.h"

#include "server/client.h"

typedef struct
{
    int sockfd;
    linked_list_t *clients;
} server_t;

server_t *server_init(int port);
void server_cleanup(server_t **server);

void server_handle_input(server_t *server);
void server_handle_output(server_t *server);

void server_send_pong_message(client_t *client);
void server_send_return_server_time_message(client_t *client, int64_t client_time);
void server_send_return_latency_message(client_t *client, int64_t client_time);
void server_send_return_entities_message(client_t *client, linked_list_t *entities);

#endif // SERVER_H
