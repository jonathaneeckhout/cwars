#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"
#include "client/client_fsm.h"

client_fsm_t *client_fsm_init()
{
    log_info("Initializing client fsm");

    client_fsm_t *client_fsm = malloc(sizeof(client_fsm_t));
    if (client_fsm == NULL)
    {
        log_error("Failed to allocate memory for client_fsm");
        return NULL;
    }
    client_fsm->running = false;

    client_fsm->state = CLIENT_STATE_INIT;

    client_fsm->game = game_init();
    if (client_fsm->game == NULL)
    {
        free(client_fsm);
        return NULL;
    }

    return client_fsm;
}

void client_fsm_cleanup(client_fsm_t **client_fsm)
{
    log_info("Cleaning up client fsm");

    if (*client_fsm == NULL)
    {
        return;
    }

    game_cleanup(&(*client_fsm)->game);

    free(*client_fsm);
    *client_fsm = NULL;
}

void client_fsm_run(client_fsm_t *fsm)
{
    fsm->running = true;

    while (fsm->running)
    {

        long start_time = get_time();
        switch (fsm->state)
        {
        case CLIENT_STATE_INIT:
            fsm->state = CLIENT_STATE_CONNECT;
            break;
        case CLIENT_STATE_CONNECT:
            if (client_connect(fsm->game->client, SERVER_ADDRESS, SERVER_PORT))
            {
                fsm->state = CLIENT_STATE_RUN;
            }
            else
            {
                fsm->state = CLIENT_STATE_DISCONNECT;
            }
            break;
        case CLIENT_STATE_DISCONNECT:
            log_info("Disconnected from server, retrying in %d second", CLIENT_RECONNECT_TIME);
            sleep(CLIENT_RECONNECT_TIME);
            fsm->state = CLIENT_STATE_CONNECT;
            break;
        case CLIENT_STATE_RUN:
            if (!fsm->game->client->connected)
            {
                fsm->state = CLIENT_STATE_DISCONNECT;
            }
            else
            {
                game_loop_once(fsm->game);
            }
            break;
        }

        long end_time = get_time();
        long elapsed_time = end_time - start_time;

        long diff = CLIENT_MICROSECONDS_PER_FRAME - elapsed_time;
        if (diff > 0)
        {
            usleep(diff);
        }
    }
}

void client_fsm_stop(client_fsm_t *fsm)
{
    fsm->running = false;
}
