#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "game.h"
#include "utils.h"

// Shut down if ctrl+c is pressed
void sig_handler(int UNUSED _sig)
{
    printf("Shutting down the game\n");

    game_stop();

    exit(0);
}

int main()
{
    // handle sig hanler
    signal(SIGINT, sig_handler);
    // Run the game
    game_run();

    return 0;
}