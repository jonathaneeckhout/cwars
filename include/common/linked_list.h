#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _link_t link_t;

struct _link_t
{
    link_t *next;
    link_t *previous;
    void *data;
};

typedef struct _linked_list_t
{
    link_t *start;
    link_t *end;
    uint32_t size;
} linked_list_t;

link_t *link_init(void *data);
void link_cleanup(link_t **link, void (*cleanup_function)(void **));

linked_list_t *linked_list_init();
void linked_list_cleanup(linked_list_t **list, void (*cleanup_function)(void **));
void linked_list_clear(linked_list_t *list, void (*cleanup_function)(void **));
bool linked_list_push_front(linked_list_t *list, void *data);
bool linked_list_append(linked_list_t *list, void *data);
bool linked_list_remove(linked_list_t *list, link_t **link, void (*cleanup_function)(void **));
void *link_get_data(link_t *link);
bool linked_list_is_empty(linked_list_t *list);
void *linked_list_pop(linked_list_t *list);

#define for_each_link(item, list) \
    for (link_t *item = list->start; item != NULL; item = item->next)

#endif
