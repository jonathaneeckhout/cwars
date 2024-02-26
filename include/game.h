#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// Function to handle input
void game_input();

// Function to update game state
void game_update();

// Function to render output
void game_render();

void game_run();

void game_stop();

#endif /* GAME_H */