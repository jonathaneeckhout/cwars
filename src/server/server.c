
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "server/server.h"

Server *server_init(int port)
{
    Server *server = malloc(sizeof(Server));
    if (server == NULL)
    {
        printf("Failed to allocate memory for server\n");
        return NULL;
    }

    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sockfd < 0)
    {
        printf("Failed to create socket\n");
        free(server);
        return NULL;
    }

    int reuse = 1;
    if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        printf("Failed to set socket option for address reuse\n");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(server->sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        printf("Failed to get socket flags\n");
        close(server->sockfd);
        free(server);
        return NULL;
    }
    flags |= O_NONBLOCK;
    if (fcntl(server->sockfd, F_SETFL, flags) < 0)
    {
        printf("Failed to set socket flags\n");
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
        printf("Failed to bind socket\n");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    if (listen(server->sockfd, 5) < 0)
    {
        printf("Failed to listen on socket\n");
        close(server->sockfd);
        free(server);
        return NULL;
    }

    printf("Server started on port %d\n", port);

    return server;
}

void server_cleanup(Server **server)
{
    if (server == NULL || *server == NULL)
    {
        return;
    }

    close((*server)->sockfd);
    free(*server);
    *server = NULL;

    printf("Server cleaned up\n");
}

void server_loop_once(Server *server)
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
            printf("Failed to accept connection\n");
            return;
        }
        return;
    }

    printf("Accepted connection\n");

    close(client_sockfd);
}