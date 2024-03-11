#ifndef SERVER_H
#define SERVER_H

#include "server/types.h"

server_t *server_init(uv_loop_t *loop, const char *addr, int port);
void server_cleanup(server_t **server);

void server_handle_input(server_t *server);
void server_handle_output(server_t *server);
void server_update(server_t *server, game_t *game, int64_t delta_time);

#endif // SERVER_H
