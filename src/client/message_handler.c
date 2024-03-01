#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "common/utils.h"
#include "common/logging.h"
#include "common/message.h"

#include "client/message_handler.h"
#include "client/client.h"

static void message_handler_parse_message(game_t *game, message_t *message)
{
    switch (message->type)
    {
    case MESSAGE_TYPE_PING:
        log_info("Received ping from %d", game->client->sockfd);

        message_t *pong_message = message_init_pong();
        if (pong_message == NULL)
        {
            log_error("Failed to initialize pong message");
            return;
        }

        linked_list_append(game->client->out_message_queue, pong_message);

        break;
    case MESSAGE_TYPE_PONG:
        log_info("Received pong from %d", game->client->sockfd);
        break;

    case MESSAGE_TYPE_RETURN_SERVER_TIME:
        log_info("Received return server time from %d", game->client->sockfd);

        message_return_server_time_response_t *response = message_return_server_time_response_deserialize(message);
        if (response == NULL)
        {
            log_error("Failed to deserialize return server time response");
            return;
        }

        game->client->latency = (get_time() - response->client_time) / 2;
        game->client->clock = response->server_time + game->client->latency;

        message_return_server_time_response_cleanup(&response);

        break;
    default:
        break;
    }
}

void message_handler_update(game_t *game, int64_t UNUSED delta_time)
{
    link_t *next_link = game->client->in_message_queue->start;
    while (next_link != NULL)
    {
        message_t *message = (message_t *)link_get_data(next_link);
        link_t *current_link = next_link;
        next_link = next_link->next;

        message_handler_parse_message(game, message);

        linked_list_remove(game->client->in_message_queue, &current_link, (void (*)(void **)) & message_cleanup);
    }
}