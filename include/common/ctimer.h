#ifndef CTIMER_H
#define CTIMER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _ctimer_t
{
    uint32_t timeout;
    uint32_t dt;
    bool interval;
    void *userdata;
    void (*callback)(void *userdata);
} ctimer_t;

ctimer_t *ctimer_init(uint32_t timeout, bool interval, void *userdata, void (*callback)(void *userdata));
void ctimer_cleanup(ctimer_t **timer);
void ctimer_update(ctimer_t *timer, uint32_t dt);

#endif