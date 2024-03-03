#include <stdlib.h>

#include "common/logging.h"
#include "common/utils.h"

#include "client/latency.h"

latency_info_t *latency_info_init()
{
    latency_info_t *latency_info = calloc(1, sizeof(latency_info_t));
    if (latency_info == NULL)
    {
        log_error("Failed to allocate memory for latency info");
        return NULL;
    }

    latency_info->latency = 0;

    return latency_info;
}

void latency_info_cleanup(latency_info_t **latency_info)
{
    if (*latency_info == NULL)
    {
        return;
    }

    free(*latency_info);
    *latency_info = NULL;
}

void latency_handle_return_latency(client_t *client, int64_t client_time)
{
    latency_info_t *latency_info = latency_info_init();
    if (latency_info == NULL)
    {
        log_error("Failed to initialize latency info");
        return;
    }

    latency_info->latency = (get_time() - client_time) / 2;
    linked_list_append(client->latency_buffer, latency_info);

    if (client->latency_buffer->size == LATENCY_BUFFER_SIZE)
    {
        int64_t total_latency = 0;
        int64_t total_counted = 0;

        linked_list_t *latency_buffer = client->latency_buffer;
        int64_t latencies[LATENCY_BUFFER_SIZE];
        int index = 0;

        // Store latencies in an array for sorting
        for_each_link(link, latency_buffer)
        {
            latencies[index++] = ((latency_info_t *)link_get_data(link))->latency;
        }

        // Sort the latencies
        qsort(latencies, LATENCY_BUFFER_SIZE, sizeof(int64_t), compare_int64);

        // Calculate the first quartile (Q1) and third quartile (Q3)
        int64_t Q1 = latencies[LATENCY_BUFFER_SIZE / 4];
        int64_t Q3 = latencies[LATENCY_BUFFER_SIZE * 3 / 4];

        // Calculate the interquartile range (IQR)
        int64_t IQR = Q3 - Q1;

        // Calculate the lower and upper bounds for what is not an outlier
        int64_t lower_bound = Q1 - 1.5 * IQR;
        int64_t upper_bound = Q3 + 1.5 * IQR;

        // Calculate the total latency and count, excluding outliers
        for (int i = 0; i < LATENCY_BUFFER_SIZE; i++)
        {
            if (latencies[i] >= lower_bound && latencies[i] <= upper_bound)
            {
                total_latency += latencies[i];
                total_counted++;
            }
        }

        // Calculate the average latency, excluding outliers
        int64_t average_latency = total_latency / total_counted;

        client->delta_latency = average_latency - client->latency;
        client->latency = average_latency;

        linked_list_clear(client->latency_buffer, (void (*)(void **))latency_info_cleanup);
    }
}

void latency_update(client_t *client, int64_t delta_time)
{
    client->clock += delta_time + client->delta_latency;
    client->delta_latency = 0;
}
