#include <stdio.h>
#include <sys/time.h>

long get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int generate_unqiue_id()
{
    static int id = 0;
    return id++;
}
