#include "resource.h"

#include <stdlib.h>

void
free_resource_error(Resource_Error *err)
{
    if (err->message) {
        free(err->message);
        err->message = NULL;
    }
}

void
resource_set_add(Resource_Set *set, Resource resource)
{
    if (set->set == NULL) {
        set->set = calloc(4, sizeof(Resource));
        set->count = 0;
        set->capacity = 4;
    }

    if (set->count >= set->capacity) {
        set->capacity *= 2;
        set->set = realloc(set->set, set->capacity * sizeof(Resource));
    }

    set->set[set->count++] = resource;
}

void
resource_set_free(Resource_Set *set)
{
    for (size_t i = 0; i < set->count; ++i) {
        free(set->set[i].resource);
    }
    free(set->set);
    set->set = NULL;
    set->count = set->capacity = 0;
}
