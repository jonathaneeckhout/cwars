#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#include <common/linked_list.h>

typedef struct
{
    linked_list_t *entities;
    uint32_t width;
    uint32_t height;
} map_t;

map_t *map_init(uint32_t width, uint32_t height);
void map_cleanup(map_t **map);

void map_add_entity(map_t *map, void *entity);
void map_remove_entity(map_t *map, void *entity);

void map_print_all_entities(map_t *map);

#endif // MAP_H