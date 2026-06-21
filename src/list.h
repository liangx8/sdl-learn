/* list.h - public header for simple list implementation
 * This header provides the opaque LIST type and the public API
 * implemented in list.c
 */

#ifndef SDL_LEARN_LIST_H
#define SDL_LEARN_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _LIST LIST;

/* Create a list capable of holding 'capacity' pointers. Returns NULL on failure
 * or if capacity is zero.
 */
LIST* list_create(int capacity);

/* Push an item into the list. Returns 1 on success, 0 on failure. */
int list_push(LIST *list,const void *item);

/* Pop the last pushed item from the list; returns the item or NULL if empty. */
const void* list_pop(LIST *list);

/* Get the last item without removing it; returns NULL if empty. */
const void* list_get_last(const LIST *list);

/* Destroy the list and free its memory. */
void list_destroy(LIST *list);

#ifdef __cplusplus
}
#endif

#endif /* SDL_LEARN_LIST_H */
