#ifndef ENTITY_BGTILES_H
#define ENTITY_BGTILES_H

#include "tileset.h"

struct entity_bgtiles {
    const struct tileset *tileset;
    const void *handle;
    int w, h;
    
    int (*get_tile)(const void *handle, int x, int y);
};

#endif

