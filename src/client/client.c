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

client_t *client_init()
{
    log_info("Initializing client");

    client_t *client = malloc(sizeof(client_t));
    if (client == NULL)
    {
        log_error("Failed to allocate memory for client");
        return NULL;
    }

    client->sockfd = 0;

    client->connected = false;

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

void client_loop_once(client_t *client)
{
    if (!client->connected)
    {
        return;
    }

    message_read_non_blocking(client->sockfd, client->in_message_queue, CLIENT_MAX_MESSAGES_READ, &client->connected);

    if (!client->connected)
    {
        return;
    }

    message_send_non_blocking(client->sockfd, client->out_message_queue);
}

void client_send_ping(client_t UNUSED *client)
{
    log_info("Sending ping to server");

    message_t *message = message_init_ping();
    if (message == NULL)
    {
        log_error("Failed to create message");
        return;
    }

    linked_list_append(client->out_message_queue, message);
}
