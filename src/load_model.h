#ifndef GARDEN_LOAD_MODEL_HEADER_INCLUDED
#define GARDEN_LOAD_MODEL_HEADER_INCLUDED

#include "lib/gl.h"
#include "lib/resources/resource.h"
#include "lib/resources/model.h"


typedef enum Model_Load_Error {
    Model_Load_Error_None,
    Model_Load_Error_Opening_File,
    Model_Load_Error_Reading_File,
} Model_Load_Error;


Model_Load_Error model_load(Model_Resource *resource, Resource_Error *err);


#endif
