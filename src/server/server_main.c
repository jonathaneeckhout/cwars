#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "common/logging.h"
#include "common/utils.h"
#include "server/game.h"

game_t *game = NULL;

// Shut down if ctrl+c is pressed
void sig_handler(int UNUSED _sig)
{
    log_info("Shutting down the game");

    game_stop(game);
}

int main()
{

    // Initialize the logging system
    logging_init("cwars server", LOG_DEBUG);

    log_info("Starting the server");

    // Initialize the game
    game = game_init();
    if (game == NULL)
    {
        log_error("Failed to initialize the game");
        return 1;
    }

    // handle sig hanler
    signal(SIGINT, sig_handler);

    // Run the game
    game_run(game);

    // Clean up
    game_cleanup(&game);

    // Clean up the logging system
    logging_cleanup();

    return 0;
}