#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <uv.h>

#include "common/logging.h"
#include "common/utils.h"
#include "client/client_fsm.h"

client_fsm_t *client_fsm = NULL;

// Shut down if ctrl+c is pressed
void sig_handler(int UNUSED _sig)
{
    log_info("Shutting down the game");

    client_fsm_stop(client_fsm);
}

int main()
{
    logging_init("cwars client", LOG_DEBUG);

    log_info("Starting the client");

    client_fsm = client_fsm_init();
    if (client_fsm == NULL)
    {
        log_error("Failed to initialize client fsm");
        return 1;
    }

    // handle sig hanler
    signal(SIGINT, sig_handler);

    client_fsm_run(client_fsm);

    client_fsm_cleanup(&client_fsm);

    logging_cleanup();

    return 0;
}
