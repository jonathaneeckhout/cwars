#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "common/utils.h"
#include "common/logging.h"
#include "common/message.h"

#include "server/message_handler.h"
#include "server/server.h"
#include "server/client.h"

static void message_handler_parse_message(client_t *client, message_t *message)
{
    switch (message->type)
    {
    case MESSAGE_TYPE_PING:
        log_info("Received ping from %d", client->sockfd);

        message_t *pong_message = message_init_pong();
        if (pong_message == NULL)
        {
            log_error("Failed to initialize pong message");
            return;
        }

        linked_list_append(client->out_message_queue, pong_message);

        break;
    case MESSAGE_TYPE_PONG:
        log_info("Received pong from %d", client->sockfd);

        break;
    case MESSAGE_TYPE_GET_SERVER_TIME:
        log_info("Received get server time from %d", client->sockfd);

        message_get_server_time_response_t *response = message_get_server_time_response_deserialize(message);
        if (response == NULL)
        {
            log_error("Failed to deserialize get server time response");
            return;
        }

        server_send_return_server_time_message(client, response->client_time);

        message_get_server_time_response_cleanup(&response);

        break;
    default:
        break;
    }
}

static void message_handler_update_client(game_t UNUSED *game, client_t *client, int64_t UNUSED delta_time)
{
    link_t *next_link = client->in_message_queue->start;
    while (next_link != NULL)
    {
        message_t *message = (message_t *)link_get_data(next_link);
        link_t *current_link = next_link;
        next_link = next_link->next;

        message_handler_parse_message(client, message);

        linked_list_remove(client->in_message_queue, &current_link, (void (*)(void **)) & message_cleanup);
    }
}

void message_handler_update(game_t *game, int64_t delta_time)
{
    link_t *next_link = game->server->clients->start;
    while (next_link != NULL)
    {
        client_t *client = (client_t *)link_get_data(next_link);
        next_link = next_link->next;

        message_handler_update_client(game, client, delta_time);
    }
}