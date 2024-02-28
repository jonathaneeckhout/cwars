#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "common/logging.h"
#include "common/config.h"
#include "common/utils.h"
#include "client/game.h"

// Function to handle input
static void game_input(game_t UNUSED *game)
{
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

void game_loop_once(game_t *game)
{
    game_input(game);
    game_update();
    game_render();
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

    game->client = client_init();
    if (game->client == NULL)
    {
        free(game);
        return NULL;
    }

    return game;
}

void game_cleanup(game_t **game)
{

    log_info("Cleaning up game");

    if (*game == NULL)
    {
        return;
    }

    client_cleanup(&(*game)->client);
    free(*game);
    *game = NULL;
}
