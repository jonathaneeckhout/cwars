#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common/utils.h"
#include "common/logging.h"
#include "common/message.h"

#include "client/message_handler.h"
#include "client/client.h"
#include "client/latency.h"

static bool message_handler_is_entity_in_get_entities_response(message_get_entities_response_t *response, const char *id)
{
    for (uint32_t i = 0; i < response->entity_count; i++)
    {
        if (strncmp(response->entities[i].id, id, 37) == 0)
        {
            return true;
        }
    }

    return false;
}

static void message_handler_handle_get_entities_response(game_t *game, message_get_entities_response_t *response)
{
    link_t *next_link = NULL;

    // Update already existing entities in the map and add new ones
    for (uint32_t i = 0; i < response->entity_count; i++)
    {
        entity_t *entity = map_get_entity_by_id(game->map, response->entities[i].id);
        if (entity != NULL)
        {
            entity->position = response->entities[i].position;
            entity->velocity = response->entities[i].velocity;
            entity->radius = response->entities[i].radius;
        }
        else
        {
            entity = entity_init(response->entities[i].position, response->entities[i].velocity, response->entities[i].radius);
            memcpy(entity->id, response->entities[i].id, 37);
            map_add_entity(game->map, entity);
        }
    }

    // Remove entities that are not in the response
    next_link = game->map->entities->start;
    while (next_link != NULL)
    {
        entity_t *entity = (entity_t *)next_link->data;
        next_link = next_link->next;

        if (!message_handler_is_entity_in_get_entities_response(response, entity->id))
        {
            map_remove_entity(game->map, entity);
            entity_cleanup(&entity);
        }
    }
}

static void message_handler_parse_message(game_t *game, message_t *message)
{
    switch (message->type)
    {
    case MESSAGE_TYPE_PONG:
        break;

    case MESSAGE_TYPE_RETURN_SERVER_TIME:;
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
    case MESSAGE_TYPE_RETURN_LATENCY:;
        message_return_latency_response_t *latency_response = message_return_latency_response_deserialize(message);
        if (latency_response == NULL)
        {
            log_error("Failed to deserialize return latency response");
            return;
        }

        latency_handle_return_latency(game->client, latency_response->client_time);

        message_return_latency_response_cleanup(&latency_response);

        break;

    case MESSAGE_TYPE_RETURN_ENTITIES:;
        message_get_entities_response_t *entities_response = message_return_entities_response_deserialize(message);
        if (entities_response == NULL)
        {
            log_error("Failed to deserialize return entities response");
            return;
        }

        message_handler_handle_get_entities_response(game, entities_response);

        message_return_entities_response_cleanup(&entities_response);

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