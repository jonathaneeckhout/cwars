#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void map_add_entity(map_t *map, entity_t *entity)
{
    linked_list_append(map->entities, entity);
}

void map_remove_entity(map_t *map, entity_t *entity)
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

entity_t *map_get_entity_by_id(map_t *map, const char *id)
{
    for_each_link(link, map->entities)
    {
        entity_t *entity = link_get_data(link);
        if (strncmp(entity->id, id, 37) == 0)
        {
            return entity;
        }
    }

    return NULL;
}

void map_print_all_entities(map_t *map)
{
    for_each_link(link, map->entities)
    {
        entity_t *entity = link_get_data(link);
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        vector_to_string(entity->position, buffer);

        printf("Entity id: %s\n", entity->id);
        printf("Entity at %s\n", buffer);
    }
}
