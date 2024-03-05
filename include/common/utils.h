#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#define UNUSED __attribute__((unused))

int64_t get_time();
int32_t generate_unique_id();
int compare_int64(const void *a, const void *b);

#endif // UTILS_H
