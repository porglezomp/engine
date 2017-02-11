#ifndef GARDEN_LOAD_TEXTURE_HEADER_INCLUDED
#define GARDEN_LOAD_TEXTURE_HEADER_INCLUDED

#include "lib/gl.h"


struct Resource_Error;
struct Resource_Set;
struct Texture_Resource;


bool texture_load(Texture_Resource *resource, Resource_Error *err);
bool texture_set_add(Resource_Set *set, const char *texture_fname,
                     Texture_Resource **out_texture, Resource_Error *err);


#endif
