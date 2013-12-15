#ifndef SPRITe_H
#define SPRITE_H

#include "tileset.h"

struct entity_sprite {
    const struct tileset *tileset;
    
    int tile;
};

#endif

