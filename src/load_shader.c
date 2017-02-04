#include "load_shader.h"

#include "shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Shader_Load_Error
shader_load(Shader_Resource *resource, Resource_Error *err)
{
    FILE *vert_file = fopen(resource->vert_fname, "r");
    if (vert_file == 0) {
        if (err) {
            free_resource_error(err);
            size_t size = sizeof("Error opening vertex source: ");
            size += strlen(resource->vert_fname);
            err->message = calloc(1, size);
            snprintf(err->message, size, "Error opening vertex source: %s",
                     resource->vert_fname);
        }
        return Shader_Load_Error_Opening_File;
    }

    FILE *frag_file = fopen(resource->frag_fname, "r");
    if (frag_file == 0) {
        fclose(vert_file);
        if (err) {
            free_resource_error(err);
            size_t size = sizeof("Error opening fragment source: ");
            size += strlen(resource->frag_fname);
            err->message = calloc(1, size);
            snprintf(err->message, size, "Error opening vertex source: %s",
                     resource->frag_fname);
        }
        return Shader_Load_Error_Opening_File;
    }

    fseek(vert_file, 0, SEEK_END);
    size_t vert_len = ftell(vert_file);
    rewind(vert_file);
    char *vert_source = calloc(1, vert_len);
    fread(vert_source, 1, vert_len, vert_file);
    fclose(vert_file);

    fseek(frag_file, 0, SEEK_END);
    size_t frag_len = ftell(frag_file);
    rewind(frag_file);
    char *frag_source = calloc(1, frag_len);
    fread(frag_source, 1, frag_len, frag_file);
    fclose(frag_file);

    Shader_Error shader_error = {
        .message = NULL,
        .type = Shader_Error_Unknown,
    };
    GLuint program = compile_shader(vert_source, frag_source, &shader_error);
    free(vert_source);
    free(frag_source);

    if (program == 0) {
        err->message = shader_error.message;
        return Shader_Load_Error_Compiling_Shader;
    }

    if (resource->program) {
        glDeleteProgram(resource->program);
    }

    resource->program = program;
    return Shader_Load_Error_None;
}

void
free_resource_error(Resource_Error *err)
{
    if (err->message) {
        free(err->message);
        err->message = NULL;
    }
}
