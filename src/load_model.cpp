#include "load_model.h"

#include "lib/resources/resource.h"
#include "lib/resources/model.h"

#include <assert.h>
#include <stdio.h>
#include <iso646.h>
#include <stdlib.h>
#include <string.h>


#define ERROR_INTO(LOC, MSG, NAME) {            \
        size_t len = sizeof(MSG);               \
        len += strlen(NAME);                    \
        LOC = (char*) calloc(1, len);           \
        snprintf(LOC, len, MSG "%s", NAME);     \
    }

#define REPORT_ERROR(MSG) if (err) {                          \
        free_resource_error(err);                             \
        ERROR_INTO(err->message, MSG, resource->model_fname); \
    }


Model_Load_Error
model_load(Model_Resource *resource, Resource_Error *err)
{
    FILE *file = fopen(resource->model_fname, "r");
    if (not file) {
        REPORT_ERROR("Error opening model file ");
        return Model_Load_Error_Opening_File;
    }

    size_t attrib_count;
    if (fscanf(file, " attrib count %zu", &attrib_count) != 1) {
        REPORT_ERROR("Error reading attrib count in ");
        fclose(file);
        return Model_Load_Error_Reading_File;
    }

    size_t stride;
    if (fscanf(file, " stride %zu", &stride) != 1) {
        REPORT_ERROR("Error reading stride in ");
        fclose(file);
        return Model_Load_Error_Reading_File;
    }

    int sizes[ATTRIB_MAX_COUNT];
    int offsets[ATTRIB_MAX_COUNT];
    for (size_t i = 0; i < attrib_count; ++i) {
        if (fscanf(file, " float size %d offset %d", &sizes[i], &offsets[i]) != 2) {
            REPORT_ERROR("Error reading attribute in ");
            fclose(file);
            return Model_Load_Error_Reading_File;
        }
    }

    size_t data_count;
    if (fscanf(file, " data count %zu ", &data_count) != 1) {
        REPORT_ERROR("Error reading data count in ");
        fclose(file);
        return Model_Load_Error_Reading_File;
    }

    GLfloat *data = (GLfloat*) calloc(data_count, sizeof(*data));
    for (size_t i = 0; i < data_count; ++i) {
        float value;
        if (fscanf(file, " %f", &value) != 1) {
            REPORT_ERROR("Error reading data element in ");
            fclose(file);
            free(data);
            return Model_Load_Error_Reading_File;
        }
        data[i] = value;
    }

    size_t index_count;
    if (fscanf(file, " index count %zu", &index_count) != 1) {
        REPORT_ERROR("Error reading index count in ");
        fclose(file);
        free(data);
        return Model_Load_Error_Reading_File;
    }

    GLuint *indices = (GLuint*) calloc(index_count, sizeof(*indices));
    for (size_t i = 0; i < index_count; ++i) {
        unsigned index;
        if (fscanf(file, " %u", &index) != 1) {
            REPORT_ERROR("Error reading index in ");
            fclose(file);
            free(data);
            free(indices);
            return Model_Load_Error_Reading_File;
        }
        indices[i] = index;
    }

    fclose(file);

    Vertex_Format *fmt = &resource->format;
    fmt->attribs_count = attrib_count;
    fmt->stride = stride;
    for (size_t i = 0; i < attrib_count; ++i) {
        fmt->offsets[i] = offsets[i];
        fmt->sizes[i] = sizes[i];
        fmt->types[i] = GL_FLOAT;
    }

    build_model(resource,
                data, data_count * sizeof(*data),
                indices, index_count);

    free(data);
    free(indices);

    return Model_Load_Error_None;
}

Model_Load_Error
model_set_add(Resource_Set *set, const char *model_fname,
              Model_Resource **out_model, Resource_Error *err)
{
    Model_Resource *model = (Model_Resource*) calloc(1, sizeof(*model));
    model->model_fname = model_fname;
    Resource resource = {
        Resource_Type_Model,
        model,
    };

    resource_set_add(set, resource);
    Model_Load_Error result = model_load(model, err);
    if (not result and out_model) { *out_model = model; }
    return result;
}
