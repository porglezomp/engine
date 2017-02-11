#ifndef GARDEN_LIB_RESOURCE_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_HEADER_INCLUDED

#include <stddef.h>


struct Resource_Error {
    char *message;
};

enum Resource_Type {
    Resource_Type_Shader,
    Resource_Type_Model,
    Resource_Type_Texture,
};

struct Resource {
    Resource_Type type;
    void *resource;
};

struct Resource_Set {
    size_t count, capacity;
    Resource *set;
};


void free_resource_error(Resource_Error *err);
void resource_set_add(Resource_Set *queue, Resource resource);
void resource_set_free(Resource_Set *queue);


const extern Resource_Set Resource_Set_Empty;


#endif
