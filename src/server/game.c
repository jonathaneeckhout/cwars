#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"

#include "server/game.h"
#include "server/server.h"
#include "server/server_map.h"
#include "server/message_handler.h"

static void game_input(game_t *game)
{
    server_handle_input(game->server);
}

static void game_update(game_t *game, int64_t delta_time)
{
    message_handler_update(game, delta_time);
    server_update(game->server, game, delta_time);
    map_update(game->map, game, delta_time);
}

static void game_output(game_t *game)
{
    server_handle_output(game->server);
}

void game_loop_once(game_t *game, int64_t delta_time)
{
    game_input(game);
    game_update(game, delta_time);
    game_output(game);
}

void game_run(game_t *game)
{
    game->running = true;

    int64_t last_time = get_time();

    while (game->running)
    {

        int64_t current_time = get_time();
        int64_t delta_time = current_time - last_time;

        game_loop_once(game, delta_time);

        last_time = current_time;
        int64_t elapsed_time = get_time() - current_time;

        usleep(SERVER_MILLISECONDS_PER_FRAME - elapsed_time);
    }
}

void game_stop(game_t *game)
{
    game->running = false;
}

game_t *game_init()
{
    game_t *game = calloc(1, sizeof(game_t));
    if (game == NULL)
    {
        log_error("Failed to allocate memory for game");
        return NULL;
    }

    // Seed random number generator
    srand(time(NULL));

    game->running = false;

    game->server = server_init(SERVER_PORT);
    if (game->server == NULL)
    {
        free(game);
        return NULL;
    }

    game->map = map_init(MAP_WIDTH, MAP_HEIGHT);
    if (game->map == NULL)
    {
        server_cleanup(&game->server);
        free(game);
        return NULL;
    }

    return game;
}

void game_cleanup(game_t **game)
{
    if (game == NULL || *game == NULL)
    {
        return;
    }

    map_cleanup(&(*game)->map);
    server_cleanup(&(*game)->server);
    free(*game);
    *game = NULL;
}
