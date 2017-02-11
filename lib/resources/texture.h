#ifndef GARDEN_LIB_RESOURCE_TEXTURE_HEADER_INCLUDED
#define GARDEN_LIB_RESOURCE_TEXTURE_HEADER_INCLUDED

#include "lib/gl.h"


struct Texture_Resource {
    const char *texture_fname = nullptr;
    GLuint texture_handle = 0;
};


#endif
