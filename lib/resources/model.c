#include "model.h"

#include "shader.h"

void
bind_model(Model_Resource *model)
{
    glBindVertexArray(model->vao);
    if (model->shader) {
        glUseProgram(model->shader->program);
    }
}

void
build_model(Model_Resource *model,
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

    model->index_count = index_count;

    for (size_t i = 0; i < model->format.attribs_count; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, model->format.sizes[i],
                              model->format.types[i], GL_FALSE,
                              model->format.stride,
                              (void*)model->format.offsets[i]);
    }

    for (size_t i = model->format.attribs_count; i < ATTRIB_MAX_COUNT; ++i) {
        glDisableVertexAttribArray(i);
    }

    if (model->shader) {
        glUseProgram(model->shader->program);
    }
}

Vertex_Format vertex_format_xyz_rgb = {
    .attribs_count = 2,
    .stride = sizeof(Vertex_XYZ_RGB),
    .sizes = {3, 3},
    .types = {GL_FLOAT, GL_FLOAT},
    .offsets = {offsetof(Vertex_XYZ_RGB, x), offsetof(Vertex_XYZ_RGB, r)},
};
