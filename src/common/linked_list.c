#include <stdlib.h>

#include "common/linked_list.h"
#include "common/logging.h"

link_t *link_init(void *data)
{
    link_t *link = (link_t *)calloc(1, sizeof(link_t));
    if (link == NULL)
    {
        log_warning("Could not allocate link_t memory\n\r");
        return NULL;
    }
    link->next = NULL;
    link->previous = NULL;
    link->data = data;

    return link;
}

void link_cleanup(link_t **link, void (*cleanup_function)(void **))
{
    if (cleanup_function)
    {
        (*cleanup_function)(&(*link)->data);
    }

    free(*link);
    link = NULL;
}

linked_list_t *linked_list_init()
{
    linked_list_t *list = (linked_list_t *)calloc(1, sizeof(linked_list_t));
    if (list == NULL)
    {
        log_warning("Could not allocate linked_list_t memory");
        return NULL;
    }
    list->start = NULL;
    list->end = NULL;
    list->size = 0;

    return list;
}

void linked_list_cleanup(linked_list_t **list, void (*cleanup_function)(void **))
{
    link_t *current = (*list)->start;
    while (current != NULL)
    {
        link_t *offset = current;
        current = current->next;
        link_cleanup(&offset, cleanup_function);
    }

    free(*list);
    list = NULL;
}

void linked_list_clear(linked_list_t *list, void (*cleanup_function)(void **))
{
    link_t *current = list->start;
    while (current != NULL)
    {
        link_t *offset = current;
        current = current->next;
        link_cleanup(&offset, cleanup_function);
        list->size--;
    }
    list->start = NULL;
    list->end = NULL;
}

bool linked_list_push_front(linked_list_t *list, void *data)
{
    link_t *link = link_init(data);

    if (list->size == 0)
    {
        list->start = link;
        list->end = link;
    }
    else
    {
        link->next = list->start;
        list->start->previous = link;
        list->start = link;
    }
    list->size++;

    return true;
}

bool linked_list_append(linked_list_t *list, void *data)
{
    link_t *link = link_init(data);

    if (list->size == 0)
    {
        list->start = link;
        list->end = link;
    }
    else
    {
        list->end->next = link;
        list->end = link;
    }
    list->size++;

    return true;
}

bool linked_list_remove(linked_list_t *list, link_t **link, void (*cleanup_function)(void **))
{
    link_t *current = list->start;
    while (current != NULL)
    {
        if (current == *link)
        {
            link_t *next = (*link)->next;
            link_t *previous = (*link)->previous;
            if (next != NULL)
            {
                next->previous = previous;
            }
            if (previous != NULL)
            {
                previous->next = next;
            }
            else
            {
                list->start = next;
            }
            link_cleanup(link, cleanup_function);

            list->size--;

            return true;
        }
        current = current->next;
    }

    return false;
}

void *link_get_data(link_t *link)
{
    return link->data;
}

bool linked_list_is_empty(linked_list_t *list)
{
    return list->size == 0;
}

void *linked_list_pop(linked_list_t *list)
{
    if (list->size == 0)
    {
        return NULL;
    }

    link_t *link = list->start;
    void *data = link->data;
    list->start = link->next;
    list->size--;

    link_cleanup(&link, NULL);

    return data;
}