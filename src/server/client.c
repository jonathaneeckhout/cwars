#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/message.h"
#include "common/config.h"

#include "server/client.h"

client_t *client_init(int sockfd, struct sockaddr_in addr)
{
    log_info("Initializing client");

    client_t *client = malloc(sizeof(client_t));
    if (client == NULL)
    {
        log_error("Failed to allocate memory for client");
        return NULL;
    }

    client->sockfd = sockfd;
    client->addr = addr;

    client->connected = true;
    client->logged_in = false;

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
    log_info("Cleaning up client");

    if (*client == NULL)
    {
        return;
    }

    if ((*client)->sockfd > 0)
    {
        close((*client)->sockfd);
    }

    linked_list_cleanup(&(*client)->out_message_queue, (void (*)(void **)) & message_cleanup);
    linked_list_cleanup(&(*client)->in_message_queue, (void (*)(void **)) & message_cleanup);

    free(*client);
    *client = NULL;
}

void client_handle_input(client_t *client)
{
    if (!client->connected)
    {
        return;
    }

    message_read_non_blocking(client->sockfd, client->in_message_queue, SERVER_MAX_MESSAGES_READ, &client->connected);
}

void client_handle_output(client_t *client)
{
    if (!client->connected)
    {
        return;
    }

    message_send_non_blocking(client->sockfd, client->out_message_queue);
}
