#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"

#include "client/game.h"
#include "client/message_handler.h"

static void game_input(game_t *game)
{
    client_handle_input(game->client);
}

static void game_update(game_t *game, int64_t delta_time)
{
    ctimer_update(game->latency_timer, delta_time);
    message_handler_update(game, delta_time);
    client_update(game->client, delta_time);
}

static void game_output(game_t *game)
{
    client_handle_output(game->client);
}

void game_loop_once(game_t *game, int64_t delta_time)
{
    game_input(game);
    game_update(game, delta_time);
    game_output(game);
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

    game->client = client_init();
    if (game->client == NULL)
    {
        free(game);
        return NULL;
    }

    game->latency_timer = ctimer_init(CLIENT_MS_PER_CLOCK_SYNC, true, game->client, (void (*)(void *)) & client_send_get_latency_message);
    if (game->latency_timer == NULL)
    {
        client_cleanup(&game->client);
        free(game);
        return NULL;
    }

    game->map = map_init(MAP_WIDTH, MAP_HEIGHT);
    if (game->map == NULL)
    {
        client_cleanup(&game->client);
        ctimer_cleanup(&game->latency_timer);
        free(game);
        return NULL;
    }

    return game;
}

void game_cleanup(game_t **game)
{
    if (*game == NULL)
    {
        return;
    }

    client_cleanup(&(*game)->client);
    ctimer_cleanup(&(*game)->latency_timer);
    map_cleanup(&(*game)->map);

    free(*game);
    *game = NULL;
}
