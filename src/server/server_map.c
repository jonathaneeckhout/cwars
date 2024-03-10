#include <stdlib.h>

#include "server/server_map.h"
#include "server/server_entity.h"

void map_update(map_t *map, game_t *game, int64_t delta_time)
{
    for_each_link(link, map->entities)
    {
        entity_t *entity = link_get_data(link);
        entity_update(entity, game, delta_time);
    }
}