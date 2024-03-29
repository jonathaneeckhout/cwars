
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "common/logging.h"
#include "common/utils.h"
#include "common/message.h"

#include "server/server.h"
#include "server/client.h"

server_t *server_init(int port)
{
    server_t *server = calloc(1, sizeof(server_t));
    if (server == NULL)
    {
        log_error("Failed to allocate memory for server");
        return NULL;
    }

    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sockfd < 0)
    {
        log_error("Failed to create socket");
        free(server);
        return NULL;
    }

    int reuse = 1;
    if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        log_error("Failed to set socket option for address reuse");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    int flags = fcntl(server->sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        log_error("Failed to get socket flags");
        close(server->sockfd);
        free(server);
        return NULL;
    }
    flags |= O_NONBLOCK;
    if (fcntl(server->sockfd, F_SETFL, flags) < 0)
    {
        log_error("Failed to set socket flags to non-blocking");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        log_error("Failed to bind socket to port %d\n", port);
        close(server->sockfd);
        free(server);
        return NULL;
    }

    if (listen(server->sockfd, 5) < 0)
    {
        log_error("Failed to listen on socket");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    server->clients = linked_list_init();
    if (server->clients == NULL)
    {
        log_error("Failed to initialize clients list");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    log_info("Server started on port %d", port);

    return server;
}

void server_cleanup(server_t **server)
{
    if (server == NULL || *server == NULL)
    {
        return;
    }

    linked_list_cleanup(&(*server)->clients, (void (*)(void **)) & client_cleanup);

    close((*server)->sockfd);
    free(*server);
    *server = NULL;
}

static void server_check_for_incomming_clients(server_t *server)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int flags = 0;
    client_t *client = NULL;

    int client_sockfd = accept(server->sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockfd < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            return;
        }
        else
        {
            log_warning("Failed to accept connection");
            return;
        }
        return;
    }

    log_info("New client connected");

    // Set client socket to non-blocking
    flags = fcntl(client_sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        log_error("Failed to get socket flags");
        close(client_sockfd);
        return;
    }
    flags |= O_NONBLOCK;
    if (fcntl(client_sockfd, F_SETFL, flags) < 0)
    {
        log_error("Failed to set socket flags to non-blocking");
        close(client_sockfd);
        return;
    }

    client = client_init(client_sockfd, client_addr);
    if (client == NULL)
    {
        log_error("Failed to initialize client");
        close(client_sockfd);
        return;
    }

    linked_list_append(server->clients, client);
}

static void server_remove_client(server_t *server, client_t *client)
{
    link_t *next_link = server->clients->start;
    while (next_link != NULL)
    {
        client_t *current_client = (client_t *)link_get_data(next_link);
        link_t *current_link = next_link;
        next_link = next_link->next;

        if (current_client == client)
        {
            linked_list_remove(server->clients, &current_link, (void (*)(void **)) & client_cleanup);
            return;
        }
    }
}

static void server_handle_clients_input(server_t *server)
{
    link_t *next_link = server->clients->start;
    while (next_link != NULL)
    {
        bool error = false;

        client_t *client = (client_t *)link_get_data(next_link);
        next_link = next_link->next;

        while (!error && message_read_input(client->sockfd, client->in_message_queue, &client->incomming_message, &error))
            ;

        if (error)
        {
            log_warning("Failed to receive message, disconnecting client");
            server_remove_client(server, client);
        }
    }
}

static void server_handle_clients_output(server_t *server)
{
    link_t *next_link = server->clients->start;
    while (next_link != NULL)
    {
        bool error = false;

        client_t *client = (client_t *)link_get_data(next_link);
        next_link = next_link->next;

        while (!error && message_write_output(client->sockfd, client->out_message_queue, &client->outgoing_message, &error))
            ;

        if (error)
        {
            log_warning("Failed to send message, disconnecting client");
            server_remove_client(server, client);
        }
    }
}

void server_handle_input(server_t *server)
{
    server_check_for_incomming_clients(server);
    server_handle_clients_input(server);
}

void server_handle_output(server_t *server)
{
    server_handle_clients_output(server);
}

void server_update(server_t *server, game_t *game, int64_t delta_time)
{
    link_t *next_link = server->clients->start;
    while (next_link != NULL)
    {
        client_t *client = (client_t *)link_get_data(next_link);
        next_link = next_link->next;

        client_update(client, game, delta_time);
    }
}
