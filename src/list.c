#include <stddef.h>
#include <stdlib.h>

typedef struct _LIST {
    int tail;         // Number of elements currently stored in the list
    int capacity;     // Maximum number of elements the list can hold
    void *data[];     // Flexible array of void* data elements
} LIST;

LIST* list_create(int capacity) {
    if (capacity == 0) {
        return NULL; // Cannot create a list with zero capacity
    }
    LIST *list = malloc(sizeof(LIST) + sizeof(void*) * capacity);
    if (!list) {
        return NULL; // Memory allocation failed
    }
    list->tail = 0;
    list->capacity = capacity;
    return list;
}

int list_push(LIST *list, void *item) {
    if (!list || list->tail < 0 || list->tail >= list->capacity) {
        return 0;
    }
    list->data[list->tail++] = item;
    return 1;
}

void* list_pop(LIST *list) {
    if (!list || list->tail <= 0) {
        return NULL;
    }
    void *item = list->data[--list->tail];
    list->data[list->tail] = NULL;
    return item;
}

void* list_get_last(const LIST *list) {
    if (!list || list->tail <= 0) {
        return NULL;
    }
    return list->data[list->tail - 1];
}

void list_destroy(LIST *list) {
    free(list);
}