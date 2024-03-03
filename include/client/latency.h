#ifndef LATENCY_H
#define LATENCY_H

#include <stdint.h>

#include "client/client.h"

#define LATENCY_BUFFER_SIZE 9
#define LATENCY_BUFFER_MID_POINT LATENCY_BUFFER_SIZE / 2
#define LATENCY_MINIMUM_THRESHOLD = 20

typedef struct
{
    int64_t latency;
} latency_info_t;

latency_info_t *latency_info_init();
void latency_info_cleanup(latency_info_t **latency_info);

void latency_handle_return_latency(client_t *client, int64_t client_time);

void latency_update(client_t *client, int64_t delta_time);

#endif // LATENCY_H