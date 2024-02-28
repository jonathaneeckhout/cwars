
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
#include "server/server.h"
#include "server/client.h"

Server *server_init(int port)
{
    log_info("Initializing server");

    Server *server = malloc(sizeof(Server));
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

void server_cleanup(Server **server)
{
    log_info("Cleaning up server");

    if (server == NULL || *server == NULL)
    {
        return;
    }

    linked_list_cleanup(&(*server)->clients, (void (*)(void **)) & client_cleanup);

    close((*server)->sockfd);
    free(*server);
    *server = NULL;

    printf("Server cleaned up\n");
}

void server_check_for_incomming_clients(Server *server)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
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
    int flags = fcntl(client_sockfd, F_GETFL, 0);
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

    Client *client = client_init(client_sockfd, client_addr);
    if (client == NULL)
    {
        log_error("Failed to initialize client");
        close(client_sockfd);
        return;
    }

    linked_list_append(server->clients, client);
}

void server_handle_clients(Server *server)
{
    link_t *next_link = server->clients->start;
    while (next_link != NULL)
    {
        Client *client = (Client *)link_get_data(next_link);
        link_t *current_link = next_link;
        next_link = next_link->next;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        int n = read(client->sockfd, buffer, sizeof(buffer));
        if (n < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                continue;
            }
            else
            {
                log_warning("Failed to read from client");
                linked_list_remove(server->clients, &current_link, (void (*)(void **)) & client_cleanup);
                continue;
            }
        }
        else if (n == 0)
        {
            log_info("Client disconnected");
            linked_list_remove(server->clients, &current_link, (void (*)(void **)) & client_cleanup);
            continue;
        }

        log_info("Received message from client: %s\n", buffer);
    }
}

void server_loop_once(Server *server)
{
    server_check_for_incomming_clients(server);
    server_handle_clients(server);
}