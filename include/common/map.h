#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#include <common/linked_list.h>
#include <common/entity.h>

typedef struct
{
    linked_list_t *entities;
    uint32_t width;
    uint32_t height;
} map_t;

map_t *map_init(uint32_t width, uint32_t height);
void map_cleanup(map_t **map);

void map_add_entity(map_t *map, entity_t *entity);
void map_remove_entity(map_t *map, entity_t *entity);
entity_t *map_get_entity_by_id(map_t *map, const char *id);

void map_print_all_entities(map_t *map);

#endif // MAP_H