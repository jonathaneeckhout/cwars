#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "server/config.h"
#include "common/utils.h"
#include "server/game.h"

static bool running = true;

// Function to handle input
static void game_input()
{
    // TODO: Implement input handling logic
}

// Function to update game state
static void game_update()
{
    printf("Updating game state\n");
}

// Function to render output
static void game_render()
{
    // TODO: Implement rendering logic
}

void game_run(Game *game)
{
    game->running = true;

    while (running)
    {

        long start_time = get_time();

        game_input();
        game_update();
        game_render();

        long end_time = get_time();
        long elapsed_time = end_time - start_time;

        usleep(MICROSECONDS_PER_FRAME - elapsed_time);
    }
}

void game_stop(Game *game)
{
    game->running = false;
}

Game *game_init()
{
    Game *game = malloc(sizeof(Game));
    if (game == NULL)
    {
        perror("Failed to allocate memory for game");
        return NULL;
    }

    game->running = false;

    game->server = server_init(PRIVATE_KEY_FILE, CERTIFICATE_FILE);
    if (game->server == NULL)
    {
        free(game);
        return NULL;
    }

    return game;
}

void game_cleanup(Game **game)
{
    server_cleanup(&(*game)->server);
    free(*game);
    *game = NULL;
}
