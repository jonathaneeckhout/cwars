#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "client/game.h"
#include <stdint.h>

void message_handler_update(game_t *game, int64_t delta_time);

#endif // MESSAGE_HANDLER_H
