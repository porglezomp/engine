#include "load_shader.h"
#include "lib/resources/resource.h"
#include "lib/resources/shader.h"

#include <stdlib.h>
#include <iso646.h>
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
            err->message = (char*) calloc(1, size);
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
            err->message = (char*) calloc(1, size);
            snprintf(err->message, size, "Error opening vertex source: %s",
                     resource->frag_fname);
        }
        return Shader_Load_Error_Opening_File;
    }

    fseek(vert_file, 0, SEEK_END);
    size_t vert_len = ftell(vert_file);
    rewind(vert_file);
    char *vert_source = (char*) calloc(1, vert_len);
    fread(vert_source, 1, vert_len, vert_file);
    fclose(vert_file);

    fseek(frag_file, 0, SEEK_END);
    size_t frag_len = ftell(frag_file);
    rewind(frag_file);
    char *frag_source = (char*) calloc(1, frag_len);
    fread(frag_source, 1, frag_len, frag_file);
    fclose(frag_file);

    Shader_Error shader_error = {
        NULL,
        Shader_Error_Unknown,
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

GLuint
compile_shader_part(const char *src, GLenum shader_type, Shader_Error *err)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE) {
        if (err) {
            free_shader_error(err);
            GLint max_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
            err->message = (char*) calloc(1, max_length);

            switch (shader_type) {
            case GL_VERTEX_SHADER:
                err->type = Shader_Error_Vert;
                break;
            case GL_FRAGMENT_SHADER:
                err->type = Shader_Error_Frag;
                break;
            default:
                err->type = Shader_Error_Unknown;
                break;
            }

            glGetShaderInfoLog(shader, max_length, NULL, err->message);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint
compile_shader(const char *vert_src, const char *frag_src,
               Shader_Error *err)
{
    GLuint vert_shader = compile_shader_part(vert_src, GL_VERTEX_SHADER, err);
    if (vert_shader == 0) {
        return 0;
    }

    GLuint frag_shader = compile_shader_part(frag_src, GL_FRAGMENT_SHADER, err);
    if (frag_shader == 0) {
        glDeleteShader(vert_shader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (is_linked == GL_FALSE) {
        if (err) {
            free_shader_error(err);
            GLint max_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
            err->message = (char*) calloc(1, max_length);

            glGetProgramInfoLog(program, max_length, NULL, err->message);
        }

        glDeleteProgram(program);
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        return 0;
    }

    glDetachShader(program, vert_shader);
    glDetachShader(program, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return program;
}

void
free_shader_error(Shader_Error *err)
{
    if (err->message) {
        free(err->message);
        err->message = NULL;
    }
}

Shader_Load_Error
shader_set_add(Resource_Set *set,
               const char *vert_fname, const char *frag_fname,
               Shader_Resource **out_shader, Resource_Error *err)
{
    Shader_Resource *shader = (Shader_Resource*) calloc(1, sizeof(*shader));
    shader->vert_fname = vert_fname;
    shader->frag_fname = frag_fname;
    shader->program = 0;
    Resource resource = {
        Resource_Type_Shader,
        shader,
    };

    resource_set_add(set, resource);
    Shader_Load_Error result = shader_load(shader, err);
    if (not result and out_shader) { *out_shader = shader; }
    return result;
}
