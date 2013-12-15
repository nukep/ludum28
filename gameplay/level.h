#ifndef GAMEPLAY_LEVEL_H
#define GAMEPLAY_LEVEL_H

#include "entities.h"

struct level_liquid {
    int x, y;
    struct entity_liquid l;
};

enum level_boulder_state {
    BS_UNACTIVATED, BS_SHAKE, BS_FALLING, BS_DOWN
};

struct level_boulder {
    int state;
    int tick;
    int vel;
    int x, y, w, h;
    int original_y;
    int range_h;
    struct entity_shape shape;
};

struct level {
    int *map_tiles;
    int *map_tiles_info;
    int width, height;
    
    int spawn_x, spawn_y;
    
    int water_count, lava_count;
    struct level_liquid *water;
    struct level_liquid *lava;
    
    int boulder_count;
    struct level_boulder *boulder;
};

void level_generate(struct level *l);
void level_fini(const struct level *l);
void level_set_tile(struct level *l, int x, int y, int tile);
int level_get_tile_info(struct level *l, int x, int y);
void level_set_tile_info(struct level *l, int x, int y, int info);

#endif

