#include <stdlib.h>
#include <stdio.h>

#include "common/map.h"
#include "common/entity.h"

map_t *map_init(uint32_t width, uint32_t height)
{
    map_t *map = calloc(1, sizeof(map_t));
    if (map == NULL)
    {
        return NULL;
    }

    map->entities = linked_list_init();
    if (map->entities == NULL)
    {
        free(map);
        return NULL;
    }

    map->width = width;
    map->height = height;

    return map;
}

void map_cleanup(map_t **map)
{
    if (map == NULL || *map == NULL)
    {
        return;
    }

    linked_list_cleanup(&(*map)->entities, (void (*)(void **)) & entity_cleanup);
    free(*map);
    *map = NULL;
}

void map_add_entity(map_t *map, void *entity)
{
    linked_list_append(map->entities, entity);
}

void map_remove_entity(map_t *map, void *entity)
{
    for_each_link(link, map->entities)
    {
        if (link_get_data(link) == entity)
        {
            linked_list_remove(map->entities, &link, NULL);
            return;
        }
    }
}

void map_print_all_entities(map_t *map)
{
    for_each_link(link, map->entities)
    {
        entity_t *entity = link_get_data(link);
        printf("Entity at (%f, %f)\n", entity->position.x, entity->position.y);
    }
}
