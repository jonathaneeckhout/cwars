#include <stdlib.h>
#include <stdio.h>

#include "server/client.h"

Client *client_init(int sockfd, struct sockaddr_in addr)
{
    Client *client = malloc(sizeof(Client));
    if (client == NULL)
    {
        printf("Failed to allocate memory for client\n");
        return NULL;
    }

    client->sockfd = sockfd;
    client->addr = addr;

    return client;
}

void client_cleanup(Client **client)
{
    if (*client != NULL)
    {
        free(*client);
        *client = NULL;
    }
}