#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "server/game.h"
#include "common/utils.h"

Game *game = NULL;

// Shut down if ctrl+c is pressed
void sig_handler(int UNUSED _sig)
{
    printf("Shutting down the game\n");

    game_stop(game);
}

int main()
{
    // Initialize the game
    game = game_init();
    if (game == NULL)
    {
        perror("Failed to initialize the game");
        return 1;
    }

    // handle sig hanler
    signal(SIGINT, sig_handler);

    // Run the game
    game_run(game);

    // Clean up
    game_cleanup(&game);

    return 0;
}