#include "resource.h"

#include <stdlib.h>

void
free_resource_error(Resource_Error *err)
{
    if (err->message) {
        free(err->message);
        err->message = NULL;
    }
}
