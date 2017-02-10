#ifndef GARDEN_IMAGE_HEADER_INCLUDED
#define GARDEN_IMAGE_HEADER_INCLUDED

#include <stddef.h>


enum class Pixel_Spec {
    BGR,
    BGRA,
};

struct Image {
    size_t width = 0, height = 0;
    char *data = nullptr;
    Pixel_Spec pixel_spec;
};


#endif
