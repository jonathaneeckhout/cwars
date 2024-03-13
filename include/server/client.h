#ifndef CLIENT_H
#define CLIENT_H

#include "server/types.h"

client_t *client_init(int sockfd, struct sockaddr_in addr);
void client_cleanup(client_t **client);

void client_update(client_t *client, game_t *game, int64_t delta_time);

void client_send_pong_message(client_t *client);
void client_send_return_server_time_message(client_t *client, int64_t client_time);
void client_send_return_latency_message(client_t *client, int64_t client_time);
void client_send_return_entities_message(client_t *client, linked_list_t *entities);

#endif // CLIENT_H
