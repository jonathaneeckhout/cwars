#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"
#include "server/game.h"

static void game_input(game_t *game)
{
    server_handle_input(game->server);
}

static void game_update(game_t UNUSED *game)
{
    // log_info("Updating game state");
}

static void game_output(game_t *game)
{
    server_handle_output(game->server);
}

void game_run(game_t *game)
{
    game->running = true;

    while (game->running)
    {

        long start_time = get_time();

        game_input(game);
        game_update(game);
        game_output(game);

        long end_time = get_time();
        long elapsed_time = end_time - start_time;

        usleep(MICROSECONDS_PER_FRAME - elapsed_time);
    }
}

void game_stop(game_t *game)
{
    game->running = false;
}

game_t *game_init()
{
    log_info("Initializing game");

    game_t *game = malloc(sizeof(game_t));
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

void game_cleanup(game_t **game)
{
    log_info("Cleaning up game");

    if (game == NULL || *game == NULL)
    {
        return;
    }

    server_cleanup(&(*game)->server);
    free(*game);
    *game = NULL;
}
