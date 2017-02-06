#ifndef GARDEN_LIB_RESOURCE_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_HEADER_INCLUDED

#include <stddef.h>


typedef struct Resource_Error {
    char *message;
} Resource_Error;

typedef enum Resource_Type {
    Resource_Type_Shader,
    Resource_Type_Model
} Resource_Type;

typedef struct Resource {
    Resource_Type type;
    void *resource;
} Resource;

typedef struct Resource_Set {
    size_t count, capacity;
    Resource *set;
} Resource_Set;


void free_resource_error(Resource_Error *err);
void resource_set_add(Resource_Set *queue, Resource resource);
void resource_set_free(Resource_Set *queue);


const extern Resource_Set Resource_Set_Empty;


#endif
