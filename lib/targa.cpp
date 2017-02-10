#include "targa.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Image
load_targa(const char *fname, Resource_Error *err)
{
    FILE *f = fopen(fname, "r");
    if (f == NULL) {
        if (err) {
            free_resource_error(err);
            size_t len = strlen("Error opening file ") + strlen(fname);
            err->message = (char*) calloc(1, len + 1);
            snprintf(err->message, len, "Error opening file %s", fname);
        }
        return {};
    }

    char image_id_len = fgetc(f);
    char map_type = fgetc(f);

    if (map_type != 0) {
        if (err) {
            free_resource_error(err);
            size_t len = sizeof("Unsupported color map type 000, only 0 is supported");
            err->message = (char*) calloc(1, len);
            snprintf(err->message, len,
                     "Unsupported color map type %d, only 0 is supported",
                     map_type);
        }
        return {};
    }

    char image_type = fgetc(f);

    if (image_type != 2) {
        if (err) {
            free_resource_error(err);
            size_t len = sizeof("Unsupported image type 000, only 2 is supported");
            err->message = (char*) calloc(1, len);
            snprintf(err->message, len,
                     "Unsupported image type %d, only 2 is supported",
                     image_type);
        }
        return {};
    }

    fseek(f, 12, SEEK_SET);
    uint16_t width, height;
    Image image;
    fread(&width, 1, sizeof(width), f);
    fread(&height, 1, sizeof(height), f);
    image.width = width;
    image.height = height;
    char pixel_size = fgetc(f);
    if (pixel_size == 24) {
        image.pixel_spec = Pixel_Spec::BGR;
    } else if (pixel_size == 32) {
        image.pixel_spec = Pixel_Spec::BGRA;
    } else {
        if (err) {
            free_resource_error(err);
            size_t len = sizeof("Unsupported pixel size 000, only 24 and 32 supported");
            err->message = (char*) calloc(1, len);
            snprintf(err->message, len,
                     "Unsupported pixel size %d, only 24 and 32 supported",
                     pixel_size);
        }
        return {};
    }
    pixel_size /= 8;
    char image_desc = fgetc(f);
    bool image_origin_top = image_desc & (1 << 5);
    image.data = (char*) calloc(1, image.width * image.height * pixel_size);

    // Skip to the body of the image
    fseek(f, 18 + image_id_len, SEEK_SET);
    // We read with the bottom row first because that's what OpenGL expects
    if (image_origin_top) {
        for (size_t row = 0; row < image.height; ++row) {
            fread(&image.data[(image.height - row - 1) * image.width * pixel_size],
                  1, image.width * pixel_size, f);
        }
    } else {
        fread(image.data, 1, image.width * image.height * pixel_size, f);
    }

    return image;
}
