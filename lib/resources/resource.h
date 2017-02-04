#ifndef GARDEN_LIB_RESOURCE_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_HEADER_INCLUDED


typedef struct Resource_Error {
    char *message;
} Resource_Error;

void free_resource_error(Resource_Error *err);


#endif
