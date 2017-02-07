#include "entity.h"

#include <stdlib.h>


/*
  name Example
  id 1234
  pos 1.2345 13.0 42.0
  orientation 0.0 0.0 0.0 1.0
*/

void
entity_load_string(Entity *e, const char *data)
{
    sscanf(data, "name %32s id %d pos %f %f %f orientation %f %f %f %f",
           e->name, &e->id,
           &e->pos.x, &e->pos.y, &e->pos.z,
           &e->orientation.x, &e->orientation.y, &e->orientation.z,
           &e->orientation.w);
}

void
entity_load_file(Entity *e, FILE *f)
{
    fscanf(f, "name %32s id %d pos %f %f %f orientation %f %f %f %f",
           e->name, &e->id,
           &e->pos.x, &e->pos.y, &e->pos.z,
           &e->orientation.x, &e->orientation.y, &e->orientation.z,
           &e->orientation.w);
}

void
entity_store_file(const Entity *e, FILE *f)
{
    fprintf(f, "name %s\nid %d\npos %f %f %f\norientation %f %f %f %f\n",
            e->name, e->id,
            e->pos.x, e->pos.y, e->pos.z,
            e->orientation.x, e->orientation.y, e->orientation.z,
            e->orientation.w);
}
