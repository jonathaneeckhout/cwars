#ifndef CLIENT_FSM_H
#define CLIENT_FSM_H

#include <stdbool.h>

#include "client/game.h"

// Define the states of the FSM
typedef enum
{
    CLIENT_STATE_INIT,
    CLIENT_STATE_CONNECT,
    CLIENT_STATE_DISCONNECT,
    CLIENT_STATE_RUN,
} client_fsm_state_t;

typedef struct
{
    bool running;
    client_fsm_state_t state;
    game_t *game;
} client_fsm_t;

client_fsm_t *client_fsm_init();
void client_fsm_cleanup(client_fsm_t **fsm);
void client_fsm_run(client_fsm_t *fsm);
void client_fsm_stop(client_fsm_t *fsm);

#endif // CLIENT_FSM_H
