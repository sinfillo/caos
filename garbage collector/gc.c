#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*FinalizerT)(void *ptr, size_t size);

struct Allocation {
    void *ptr;
    size_t size;
    FinalizerT finalizer;
    bool alive;
    struct Allocation *next;
};

bool points_to(void *ptr, struct Allocation *a) {
    uintptr_t uptr = (uintptr_t)ptr, aptr = (uintptr_t)a->ptr;
    return (uptr >= aptr) && (uptr - aptr <= a->size);
}

static uintptr_t stack_bottom;
static struct Allocation *node;

void gc_init(char **argv) {
    stack_bottom = (uintptr_t)(argv);
    node = NULL;
}

void *gc_malloc(size_t size, FinalizerT finalizer) {
    struct Allocation *cur =
        (struct Allocation *)malloc(sizeof(struct Allocation));
    if (cur == NULL) {
        return NULL;
    }
    cur->size = size;
    cur->ptr = malloc(size);
    cur->finalizer = finalizer;
    cur->alive = false;
    if (cur->ptr == NULL) {
        free(cur);
        return NULL;
    }
    cur->next = node;
    node = cur;
    return cur->ptr;
}

static void dfs(uintptr_t start, uintptr_t end) {
    if (start % (alignof(void *))) {
        start = start + (alignof(void *)) - start % (alignof(void *));
    }
    for (uintptr_t i = start; i + sizeof(void *) <= end; i += alignof(void *)) {
        for (struct Allocation *cur = node; cur != NULL; cur = cur->next) {
            void **i_ptr = (void **)i;
            if (cur->alive == false && points_to(*i_ptr, cur)) {
                cur->alive = true;
                dfs((uintptr_t)cur->ptr, (uintptr_t)cur->ptr + cur->size);
            }
        }
    }
}

void gc_collect_impl(uintptr_t stack_top) {
    for (struct Allocation *cur = node; cur != NULL; cur = cur->next) {
        cur->alive = false;
    }
    uintptr_t start = stack_top;
    dfs(start + sizeof(uintptr_t), stack_bottom);
    struct Allocation **ptr = &node;
    while (ptr != NULL && *ptr != NULL) {
        struct Allocation *next_elem = (*ptr)->next;
        if ((*ptr)->alive == false) {
            if ((*ptr)->finalizer != NULL) {
                (*ptr)->finalizer((*ptr)->ptr, (*ptr)->size);
            }
            free((*ptr)->ptr);
            free((*ptr));
            *ptr = next_elem;
        } else {
            ptr = &(*ptr)->next;
        }
    }
}