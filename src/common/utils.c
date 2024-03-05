#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

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