#ifndef GARDEN_HOTLOAD_HEADER_INCLUDED
#define GARDEN_HOTLOAD_HEADER_INCLUDED


typedef void (*hotload_callback)(const char *file);
void register_hotload_callback(hotload_callback callback);


#endif
