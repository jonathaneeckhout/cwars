#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "common/logging.h"
#include "common/utils.h"
#include "common/config.h"
#include "common/message.h"

#include "client/client.h"
#include "client/latency.h"

client_t *client_init()
{
    client_t *client = calloc(1, sizeof(client_t));
    if (client == NULL)
    {
        log_error("Failed to allocate memory for client");
        return NULL;
    }

    client->sockfd = 0;
    client->connected = false;
    client->clock = 0;
    client->latency = 0;
    client->delta_latency = 0;

    client->latency_buffer = linked_list_init();
    if (client->latency_buffer == NULL)
    {
        log_error("Failed to initialize latency buffer");
        free(client);
        return NULL;
    }

    client->out_message_queue = linked_list_init();
    if (client->out_message_queue == NULL)
    {
        log_error("Failed to initialize out message queue");
        linked_list_cleanup(&client->latency_buffer, (void (*)(void **)) & latency_info_cleanup);
        free(client);
        return NULL;
    }

    client->in_message_queue = linked_list_init();
    if (client->in_message_queue == NULL)
    {
        log_error("Failed to initialize in message queue");
        linked_list_cleanup(&client->latency_buffer, NULL);
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

    linked_list_cleanup(&(*client)->latency_buffer, (void (*)(void **)) & latency_info_cleanup);

    linked_list_cleanup(&(*client)->out_message_queue, (void (*)(void **)) & message_cleanup);

    linked_list_cleanup(&(*client)->in_message_queue, (void (*)(void **)) & message_cleanup);

    free(*client);
    *client = NULL;
}

bool client_connect(client_t *client, const char *address, int port)
{
    log_info("Connecting to server");

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0)
    {
        log_error("Failed to create socket");
        return NULL;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(address);

    if (connect(client->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        log_error("Failed to connect to server");
        close(client->sockfd);
        client->sockfd = 0;
        return false;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(client->sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        log_error("Failed to get socket flags");
        close(client->sockfd);
        client->sockfd = 0;
        return false;
    }
    if (fcntl(client->sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        log_error("Failed to set socket to non-blocking mode");
        close(client->sockfd);
        client->sockfd = 0;
        return false;
    }

    client->connected = true;

    log_info("Connected to server");

    return true;
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

void client_update(client_t *client, int64_t delta_time)
{
    if (client->connected)
    {
        latency_update(client, delta_time);
    }
}

void client_send_ping(client_t *client)
{
    message_t *message = message_init_ping();
    if (message == NULL)
    {
        log_error("Failed to create message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}

void client_send_get_server_time_message(client_t *client)
{
    message_t *message = message_init_get_server_time(get_time());
    if (message == NULL)
    {
        log_error("Failed to create get server time message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}

void client_send_get_latency_message(client_t *client)
{
    message_t *message = message_init_get_latency(get_time());
    if (message == NULL)
    {
        log_error("Failed to create get latency message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}

void client_send_get_entities_message(client_t *client)
{
    message_t *message = message_init_get_entities();
    if (message == NULL)
    {
        log_error("Failed to create get entities message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}