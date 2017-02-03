#include "shader.h"

#include <stdlib.h>


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
            err->message = calloc(1, max_length);

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
            err->message = calloc(1, max_length);

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
