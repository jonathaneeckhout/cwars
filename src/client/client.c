#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "common/logging.h"
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

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0)
    {
        log_error("Failed to create socket");
        free(client);
        return NULL;
    }

    return client;
}

bool client_connect(client_t *client, const char *address, int port)
{
    log_info("Connecting to server");

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(address);

    if (connect(client->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        log_error("Failed to connect to server");
        return false;
    }

    log_info("Connected to server");

    return true;
}

void client_cleanup(client_t **client)
{
    log_info("Cleaning up client");

    if (*client == NULL)
    {
        return;
    }

    close((*client)->sockfd);
    free(*client);
    *client = NULL;
}
