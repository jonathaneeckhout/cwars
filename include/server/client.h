#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#include <netinet/in.h>

typedef struct
{
    int id;
    int sockfd;
    struct sockaddr_in addr;
    bool logged_in;
} Client;

Client *client_init(int sockfd, struct sockaddr_in addr);
void client_cleanup(Client **client);

#endif // CLIENT_H
