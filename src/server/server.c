
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/utils.h"
#include "common/message.h"

#include "server/server.h"
#include "server/client.h"

static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf)
{
    if (nread == -1)
    {
        fprintf(stderr, "Read error!\n");
        uv_close((uv_handle_t *)client, NULL);
        return;
    }
    free(buf.base);
}

static void server_on_new_connection(uv_stream_t *server, int status)
{
    if (status == -1)
    {
        return;
    }

    server_t *server_data = (server_t *)server->data;

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(server_data->loop, client);
    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
    }
    else
    {
        uv_close((uv_handle_t *)client, NULL);
    }
}

server_t *server_init(uv_loop_t *loop, const char *addr, int port)
{
    server_t *server = calloc(1, sizeof(server_t));
    if (server == NULL)
    {
        log_error("Failed to allocate memory for server");
        return NULL;
    }

    server->clients = linked_list_init();
    if (server->clients == NULL)
    {
        log_error("Failed to initialize clients list");
        free(server);
        return NULL;
    }

    uv_tcp_init(loop, &server->uv_server);

    server->uv_server.data = server;

    struct sockaddr_in bind_addr;
    uv_ip4_addr(addr, port, &bind_addr);

    uv_tcp_bind(&server->uv_server, &bind_addr, 0);
    int r = uv_listen((uv_stream_t *)&server->uv_server, 128, server_on_new_connection);
    if (r)
    {
        fprintf(stderr, "Listen error!\n");
        return 1;
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

    free(*server);
    *server = NULL;
}

// static void server_check_for_incomming_clients(server_t *server)
// {
//     struct sockaddr_in client_addr;
//     socklen_t client_len = sizeof(client_addr);
//     int flags = 0;
//     client_t *client = NULL;

//     int client_sockfd = accept(server->sockfd, (struct sockaddr *)&client_addr, &client_len);
//     if (client_sockfd < 0)
//     {
//         if (errno == EWOULDBLOCK || errno == EAGAIN)
//         {
//             return;
//         }
//         else
//         {
//             log_warning("Failed to accept connection");
//             return;
//         }
//         return;
//     }

//     log_info("New client connected");

//     // Set client socket to non-blocking
//     flags = fcntl(client_sockfd, F_GETFL, 0);
//     if (flags < 0)
//     {
//         log_error("Failed to get socket flags");
//         close(client_sockfd);
//         return;
//     }
//     flags |= O_NONBLOCK;
//     if (fcntl(client_sockfd, F_SETFL, flags) < 0)
//     {
//         log_error("Failed to set socket flags to non-blocking");
//         close(client_sockfd);
//         return;
//     }

//     client = client_init(client_sockfd, client_addr);
//     if (client == NULL)
//     {
//         log_error("Failed to initialize client");
//         close(client_sockfd);
//         return;
//     }

//     linked_list_append(server->clients, client);
// }

// static void server_handle_clients_input(server_t *server)
// {
//     link_t *next_link = server->clients->start;
//     while (next_link != NULL)
//     {
//         client_t *client = (client_t *)link_get_data(next_link);
//         link_t *current_link = next_link;
//         next_link = next_link->next;

//         client_handle_input(client);

//         if (!client->connected)
//         {
//             log_info("Client disconnected");
//             linked_list_remove(server->clients, &current_link, (void (*)(void **)) & client_cleanup);
//             continue;
//         }
//     }
// }

// static void server_handle_clients_output(server_t *server)
// {
//     link_t *next_link = server->clients->start;
//     while (next_link != NULL)
//     {
//         client_t *client = (client_t *)link_get_data(next_link);
//         next_link = next_link->next;

//         client_handle_output(client);
//     }
// }

// void server_handle_input(server_t *server)
// {
//     server_check_for_incomming_clients(server);
//     server_handle_clients_input(server);
// }

// void server_handle_output(server_t *server)
// {
//     server_handle_clients_output(server);
// }

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
