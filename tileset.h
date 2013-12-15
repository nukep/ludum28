#ifndef TILESET_H
#define TILESET_H

#include "image.h"

struct tileset {
    struct image image;
    int tile_w, tile_h;
};

#endif

