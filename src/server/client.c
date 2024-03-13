#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/utils.h"
#include "common/message.h"
#include "common/config.h"

#include "server/client.h"

client_t *client_init(int sockfd, struct sockaddr_in addr)
{
    client_t *client = calloc(1, sizeof(client_t));
    if (client == NULL)
    {
        log_error("Failed to allocate memory for client");
        return NULL;
    }

    client->sockfd = sockfd;
    client->addr = addr;

    client->connected = true;
    client->logged_in = false;
    client->incomming_message = NULL;
    client->outgoing_message = NULL;

    client->out_message_queue = linked_list_init();
    if (client->out_message_queue == NULL)
    {
        log_error("Failed to initialize out message queue");
        free(client);
        return NULL;
    }

    client->in_message_queue = linked_list_init();
    if (client->in_message_queue == NULL)
    {
        log_error("Failed to initialize in message queue");
        linked_list_cleanup(&client->out_message_queue, (void (*)(void **)) & message_cleanup);
        free(client);
        return NULL;
    }

    return client;
}

void client_cleanup(client_t **client)
{
    if (*client == NULL)
    {
        return;
    }

    if ((*client)->sockfd > 0)
    {
        close((*client)->sockfd);
    }

    if ((*client)->incomming_message != NULL)
    {
        incomming_message_cleanup(&(*client)->incomming_message);
    }

    if ((*client)->outgoing_message != NULL)
    {
        outgoing_message_cleanup(&(*client)->outgoing_message);
    }

    linked_list_cleanup(&(*client)->out_message_queue, (void (*)(void **)) & message_cleanup);
    linked_list_cleanup(&(*client)->in_message_queue, (void (*)(void **)) & message_cleanup);

    free(*client);
    *client = NULL;
}

void client_update(client_t *client, game_t *game, int64_t UNUSED delta_time)
{
    if (!client->connected)
    {
        return;
    }

    client_send_return_entities_message(client, game->map->entities);
}

void client_send_pong_message(client_t *client)
{
    message_t *pong_message = message_init_pong();
    if (pong_message == NULL)
    {
        log_error("Failed to initialize pong message");
        return;
    }

    linked_list_append(client->out_message_queue, pong_message);
}

void client_send_return_server_time_message(client_t *client, int64_t client_time)
{
    message_t *message = message_init_return_server_time(get_time(), client_time);
    if (message == NULL)
    {
        log_error("Failed to create return server time message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}

void client_send_return_latency_message(client_t *client, int64_t client_time)
{
    message_t *message = message_init_return_latency(client_time);
    if (message == NULL)
    {
        log_error("Failed to create return latency message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}

void client_send_return_entities_message(client_t *client, linked_list_t *entities)
{
    entity_t entities_array[entities->size];
    int index = 0;
    message_t *message = NULL;

    memset(entities_array, 0, sizeof(entities_array));

    for_each_link(item, entities)
    {
        entity_t *entity = (entity_t *)link_get_data(item);
        entities_array[index] = *entity;
        index++;
    }

    message = message_init_return_entities(get_time(), entities->size, entities_array);
    if (message == NULL)
    {
        log_error("Failed to create return entities message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}