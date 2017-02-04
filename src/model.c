#include "model.h"

#include "load_shader.h"


void
bind_model(Model_Resource *model)
{
    glBindVertexArray(model->vao);
    glUseProgram(model->shader->program);
}

void
build_model(Model_Resource *model, Vertex_Format *vertex,
            void *data, size_t data_size,
            GLuint *indices, size_t index_count)
{
    if (model->vbo == 0)
        glGenBuffers(1, &model->vbo);
    if (model->ibo == 0)
        glGenBuffers(1, &model->ibo);
    if (model->vao == 0)
        glGenVertexArrays(1, &model->vao);

    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLuint),
                 indices, GL_STATIC_DRAW);

    for (size_t i = 0; i < vertex->attribs_count; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, vertex->sizes[i], vertex->types[i], GL_FALSE,
                              vertex->stride, (void*)vertex->offsets[i]);
    }
    for (size_t i = vertex->attribs_count; i < ATTRIB_MAX_COUNT; ++i) {
        glDisableVertexAttribArray(i);
    }

    glUseProgram(model->shader->program);
}
