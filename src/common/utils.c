#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

int64_t get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((int64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int32_t generate_unqiue_id()
{
    static int id = 0;
    return id++;
}
