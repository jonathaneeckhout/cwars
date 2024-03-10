#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <arpa/inet.h>

int64_t get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((int64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int32_t generate_unique_id()
{
    static int id = 0;
    return id++;
}

int compare_int64(const void *a, const void *b)
{
    int64_t arg1 = *(const int64_t *)a;
    int64_t arg2 = *(const int64_t *)b;

    if (arg1 < arg2)
        return -1;
    if (arg1 > arg2)
        return 1;
    return 0;
}

int64_t htonll(int64_t host)
{
#ifdef BIG_ENDIAN
    int high_part = htonl(host >> 32);
    int low_part = htonl(host & 0xFFFFFFFF);
    return ((int64_t)low_part << 32) | high_part;
#else

    return host;
#endif
}

int64_t ntohll(int64_t network)
{
#ifdef BIG_ENDIAN
    int high_part = ntohl(network >> 32);
    int low_part = ntohl(network & 0xFFFFFFFF);
    return ((int64_t)low_part << 32) | high_part;
#else
    return network;
#endif
}