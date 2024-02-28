#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"
#include "server/game.h"

// Function to handle input
static void game_input(Game *game)
{
    server_loop_once(game->server);
}

// Function to update game state
static void game_update()
{
    // log_info("Updating game state");
}

// Function to render output
static void game_render()
{
    // TODO: Implement rendering logic
}

void game_run(Game *game)
{
    game->running = true;

    while (game->running)
    {

        long start_time = get_time();

        game_input(game);
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
        log_error("Failed to allocate memory for game");
        return NULL;
    }

    game->running = false;

    game->server = server_init(SERVER_PORT);
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
