#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <netinet/in.h>

#include "common/linked_list.h"
#include "common/map.h"

typedef struct
{
    int sockfd;
    struct sockaddr_in addr;
    bool connected;
    char *username;
    bool logged_in;
    linked_list_t *out_message_queue;
    linked_list_t *in_message_queue;
} client_t;

typedef struct
{
    int sockfd;
    linked_list_t *clients;
} server_t;

typedef struct
{
    bool running;
    server_t *server;
    map_t *map;
} game_t;

#endif // TYPES_H