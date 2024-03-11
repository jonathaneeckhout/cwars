#ifndef CLIENT_H
#define CLIENT_H

#include "server/types.h"

client_t *client_init(uv_loop_t *loop);
void client_cleanup(client_t **client);

void client_handle_input(client_t *client);
void client_handle_output(client_t *client);
void client_update(client_t UNUSED *client, game_t UNUSED *game, int64_t UNUSED delta_time);

void client_send_pong_message(client_t *client);
void client_send_return_server_time_message(client_t *client, int64_t client_time);
void client_send_return_latency_message(client_t *client, int64_t client_time);
void client_send_return_entities_message(client_t *client, linked_list_t *entities);

#endif // CLIENT_H
