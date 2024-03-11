#include <stdio.h>
#include <stdlib.h>

#include "common/logging.h"
#include "common/utils.h"
#include "server/game.h"

int main()
{
    // Initialize the logging system
    logging_init("cwars server", LOG_DEBUG);

    log_info("Starting the server");

    // Initialize the game
    game_t *game = game_init();
    if (game == NULL)
    {
        log_error("Failed to initialize the game");
        return 1;
    }

    // Run the game
    game_run(game);

    // Clean up
    game_cleanup(&game);

    // Clean up the logging system
    logging_cleanup();

    return 0;
}