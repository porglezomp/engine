#include "load_texture.h"

#include "lib/resources/resource.h"
#include "lib/resources/texture.h"
#include "lib/targa.h"

#include <stdlib.h>
#include <stdio.h>


bool
texture_load(Texture_Resource *resource, Resource_Error *err)
{
    if (resource->texture_handle == 0) {
        glGenTextures(1, &resource->texture_handle);
        if (resource->texture_handle == 0) {
            if (err) {
                free_resource_error(err);
                size_t len = sizeof("Unable to generate texture");
                err->message = (char*) calloc(1, len);
                snprintf(err->message, len-1, "Unable to generate texture");
            }
            return true;
        }
    }

    glBindTexture(GL_TEXTURE_2D, resource->texture_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Image img = load_targa(resource->texture_fname, err);
    if (img.data == nullptr) { return true; }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height,
                 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
    free(img.data);

    return false;
}

bool
texture_set_add(Resource_Set *set, const char *texture_fname,
                Texture_Resource **out_texture, Resource_Error *err)
{
    Texture_Resource *texture = (Texture_Resource*) calloc(1, sizeof(*texture));
    texture->texture_fname = texture_fname;
    Resource resource = {
        Resource_Type_Texture,
        texture,
    };

    resource_set_add(set, resource);
    bool result = texture_load(texture, err);
    if (not result and out_texture) { *out_texture = texture; }
    return result;
}
