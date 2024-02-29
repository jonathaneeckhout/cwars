#include <stdlib.h>

#include "common/logging.h"
#include "common/ctimer.h"

ctimer_t *ctimer_init(uint32_t timeout, bool interval, void *userdata, void (*callback)(void *userdata))
{
    ctimer_t *timer = (ctimer_t *)calloc(1, sizeof(ctimer_t));
    if (timer == NULL)
    {
        log_error("Could not allocate timer_t memory");
        return NULL;
    }
    timer->timeout = timeout;
    timer->dt = 0;
    timer->interval = interval;
    timer->userdata = userdata;
    timer->callback = callback;
    return timer; // Added return statement
}

void ctimer_cleanup(ctimer_t **timer)
{
    if (*timer == NULL)
    {
        return;
    }

    free(*timer);
    *timer = NULL;
}

void ctimer_update(ctimer_t *timer, uint32_t dt)
{
    if (timer->dt >= timer->timeout)
    {
        timer->callback(timer->userdata);
        if (timer->interval)
        {
            timer->dt = 0;
        }
    }
    else
    {
        timer->dt += dt;
    }
}
