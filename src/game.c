#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "game.h"
#include "utils.h"

#define FPS 10
#define MICROSECONDS_PER_FRAME 1000000 / FPS

static bool running = true;

// Function to handle input
void game_input()
{
    // TODO: Implement input handling logic
}

// Function to update game state
void game_update()
{
    printf("Updating game state\n");
}

// Function to render output
void game_render()
{
    // TODO: Implement rendering logic
}

void game_run()
{
    running = true;

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

void game_stop()
{
    running = false;
}