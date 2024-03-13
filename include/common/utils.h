#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#define UNUSED __attribute__((unused))

int64_t get_time();
int32_t generate_unique_id();
int compare_int64(const void *a, const void *b);

int get_random_int(int min, int max);
float get_random_float(float min, float max);

int64_t htonll(int64_t host);
int64_t ntohll(int64_t network);

#endif // UTILS_H
