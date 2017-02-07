#ifndef GARDEN_ENTITY_HEADER_INCLUDED
#define GARDEN_ENTITY_HEADER_INCLUDED 
#include "lib/vector.h"
#include "lib/quaternion.h"
#include "lib/resources/model.h"

#include <stdio.h>


typedef struct Entity {
    char name[32];
    Vec3 pos;
    Quat orientation;
    int id;
} Entity;


void entity_load_string(Entity *entity, const char *data);
void entity_load_file(Entity *entity, FILE *f);
void entity_store_file(const Entity *entity, FILE *f);


#endif
