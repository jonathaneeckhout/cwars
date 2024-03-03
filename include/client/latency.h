#ifndef LATENCY_H
#define LATENCY_H

#include <stdint.h>

#include "client/game.h"

#define LATENCY_BUFFER_SIZE 9
#define LATENCY_BUFFER_MID_POINT LATENCY_BUFFER_SIZE / 2
#define LATENCY_MINIMUM_THRESHOLD = 20

typedef struct
{
    int64_t latency;
} latency_info_t;

latency_info_t *latency_info_init();
void latency_info_cleanup(latency_info_t **latency_info);

void latency_handle_return_latency(game_t *game, int64_t client_time);

#endif // LATENCY_H