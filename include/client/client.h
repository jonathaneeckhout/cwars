#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include <stdint.h>

#include "common/linked_list.h"

typedef struct
{
    int sockfd;
    bool connected;
    int64_t clock;
    int64_t latency;
    int64_t delta_latency;
    linked_list_t *latency_buffer;
    linked_list_t *out_message_queue;
    linked_list_t *in_message_queue;
} client_t;

client_t *client_init();
void client_cleanup(client_t **client);

bool client_connect(client_t *client, const char *address, int port);

void client_handle_input(client_t *client);
void client_handle_output(client_t *client);
void client_update(client_t *client, int64_t delta_time);

void client_send_ping(client_t *client);
void client_send_get_server_time_message(client_t *client);
void client_send_get_latency_message(client_t *client);

#endif // CLIENT_H
