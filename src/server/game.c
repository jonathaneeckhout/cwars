#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"

#include "server/game.h"
#include "server/server.h"
#include "server/server_map.h"
#include "server/message_handler.h"

static void game_signal_handler(uv_signal_t *handle, int UNUSED signum)
{
    log_info("Shutting down the game");
    game_t *game = (game_t *)handle->data;

    game_stop(game);
}

static void game_input(game_t UNUSED *game)
{
    // server_handle_input(game->server);
}

static void game_update(game_t *game, int64_t delta_time)
{
    // message_handler_update(game, delta_time);
    // server_update(game->server, game, delta_time);
    map_update(game->map, game, delta_time);
}

static void game_output(game_t UNUSED *game)
{
    // server_handle_output(game->server);
}

static void game_physics_timer_callback(uv_timer_t *handle)
{
    game_t *game = (game_t *)handle->data;
    uint64_t current_time = uv_now(game->loop);
    uint64_t delta_time = current_time - game->last_time;
    uint64_t elapsed_time = 0;

    game->last_time = current_time;

    game_input(game);
    game_update(game, delta_time);
    game_output(game);

    elapsed_time = uv_now(game->loop) - current_time;
    if (elapsed_time > MICROSECONDS_PER_FRAME)
    {
        uv_timer_start(&game->physics_timer, game_physics_timer_callback, 0, 0);
    }
    else
    {
        uv_timer_start(&game->physics_timer, game_physics_timer_callback, MICROSECONDS_PER_FRAME - elapsed_time, 0);
    }
}

void game_run(game_t *game)
{
    if (game == NULL)
    {
        return;
    }

    log_info("Running the game");

    game->running = true;

    game->last_time = uv_now(game->loop);

    uv_timer_start(&game->physics_timer, game_physics_timer_callback, MICROSECONDS_PER_FRAME, 0);

    uv_run(game->loop, UV_RUN_DEFAULT);
}

void game_stop(game_t *game)
{
    if (game == NULL)
    {
        return;
    }

    if (game->running)
    {
        log_info("Stopping the game");
        uv_stop(game->loop);
        game->running = false;
    }
}

game_t *game_init()
{
    game_t *game = calloc(1, sizeof(game_t));
    if (game == NULL)
    {
        log_error("Failed to allocate memory for game");
        return NULL;
    }

    game->loop = malloc(sizeof(uv_loop_t));
    if (game->loop == NULL)
    {
        log_error("Failed to allocate memory for uv_loop_t");
        free(game);
        return NULL;
    }

    uv_loop_init(game->loop);

    uv_signal_init(game->loop, &game->sigint);

    game->sigint.data = game;

    uv_signal_start(&game->sigint, game_signal_handler, SIGINT);

    uv_timer_init(game->loop, &game->physics_timer);

    game->physics_timer.data = game;

    game->last_time = 0;

    game->running = false;

    game->server = server_init(game->loop, SERVER_ADDRESS, SERVER_PORT);
    if (game->server == NULL)
    {
        uv_loop_close(game->loop);
        free(game->loop);
        free(game);
        return NULL;
    }

    game->map = map_init(MAP_WIDTH, MAP_HEIGHT);
    if (game->map == NULL)
    {
        uv_loop_close(game->loop);
        free(game->loop);
        server_cleanup(&game->server);
        free(game);
        return NULL;
    }

    return game;
}

void on_uv_walk(uv_handle_t *handle, void UNUSED *arg)
{
    uv_close(handle, NULL);
}

void game_cleanup(game_t **game)
{
    if (game == NULL || *game == NULL)
    {
        return;
    }

    uv_close((uv_handle_t *)&(*game)->sigint, NULL);

    // Make sure to stop the physics loop before cleaning up the server and map

    uv_close((uv_handle_t *)&(*game)->physics_timer, NULL);

    uv_run((*game)->loop, UV_RUN_ONCE);

    server_cleanup(&(*game)->server);

    map_cleanup(&(*game)->map);

    uv_loop_close((*game)->loop);

    free((*game)->loop);
    (*game)->loop = NULL;

    free(*game);
    *game = NULL;
}
