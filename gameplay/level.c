#include <stdlib.h>
#include <string.h>
#include "tiles.h"
#include "level.h"

/*
" 	c None",
".	c #000000",
"+	c #8C6810",
"@	c #B58000",
"#	c #00FF00",
"$	c #919191",
"%	c #FFFF00",
"&	c #FF00FF",
"*	c #FF0000",
"=	c #0000FF",
*/

static char *level[] = {
"            .           .+++++++++++++++++++++++++++++++++++++++",
"            .           .+++++++++++++++++++++++++++++++++++++++",
"            .           ..........++++++++++++++++++++++++++++++",
"            .                    .++++++++++++++++++++++++++++++",
" %          .               % %  .+++++...........++++++++++++++",
"ggggggg     .g@gggggg gg    % %  .+++++. $$      .++++++++++++++",
"GGGGGG.     .GGGmmmmm#mm         .+++++. $$      .++++++++++++++",
"mmmmmm.     .mmm+++++#+...........+++++.         .++++++++++++++",
"++++++.     .+++     #                 .         .++++++++++++++",
"++++++.   % .+++     #                 .         .++++++++++++++",
"++++++.     .+++     #                 .         .++++++++++++++",
"++++++.     .+++     #                 .         .++++++++++++++",
"++++++.     .+++.#..........           .         .++++++++++++++",
"++++++.     .+++ #    $$ $$            .         .++++++++++++++",
"++++++.     .+++ #    $$ $$            .         .++++++++++++++",
"++++++.     .+++ #                     .         .++++++++++++++",
"++++++.  %  .+++ #                     .         .++++++++++++++",
"++++++.     .+++ #                     . % %     .++++++++++++++",
"++++++.     .+++ #                     .         .++++++++++++++",
"+++++.       .++ #                     . % %     .++++++++++++++",
"+++++.       .+....&&&&&&&&&&....#...........#....++++++++++++++",
"+++++.       .++  .**********.   #           #                 .",
"+++++.       .+++ .**********.   #           #                 .",
"+++++.       .+++ ............   #           #                 .",
"++++.         .+++               #           #                 .",
"++++.         .+++               #           #                 .",
"++++.         .+++               #           #    %            .",
"++++.         .++++              #           #    %            .",
"+++.           .++++             #           #                 .",
"+++.           .+++++..#..............       &                 .",
"++.             .++++  #                                       .",
"++.              .+++  #                                       .",
"+.                .++  #                                       .",
"+.                 .+  #                                       .",
"+.           %      .  #                                       .",
".                   .  #                                       .",
".    %          %   .  #      %   %  %                         .",
".                    .........................*******........#..",
".                           $$$$$$$           .......  $$$$$ # .",
".                           $$$$$$$                    $$$$$ # .",
".                                                            # .",
".                                                            # .",
". %                                                          # .",
".                                                            # .",
".                                                            # .",
".                        %                                   # .",
".    .                                                %      # .",
".....................................======........*********....",
"++++++++++++++++++++++++++++++++++++.======.++++++.*********.+++",
"++++++++++++++++++++++++++++++++++++.======.++++++.*********.+++",
"+++++++++++++++++++++++++.......++++.=====..++++++...........+++",
"+++++++++++++++++++++++++.=====.++++.======.++++++++++++++++++++",
"+++++++++++++++++++++++++.=%=%=.++++.======.++++++++++++++++++++",
"+++++++++++++++++++++++++.=====.......=====.++++++++++++++++++++",
"+++++++++++++++++++++++++.=%=%=============.++++++++++++++++++++",
"+++++++++++++++++++++++++.=================.++++++++++++++++++++",
"+++++++++++++++++++++++++.=%=%=============.++++++++++++++++++++",
"+++++++++++++++++++++++++.=================.++++++++++++++++++++",
"+++++++++++++++++++++++++............======.++++++++++++++++++++",
"++++++++++++++++++++++++++++++++++++..=====.++++++++++++++++++++",
"++++++++++++++++++++++++++++++++++++.======.++++++++++++++++++++",
"++++++++++++++++++++++++++++++++++++.======.++++++++++++++++++++",
"++++++++++++++++++++++++++++++++++++.======.++++++++++++++++++++",
"++++++++++++++++++++++++++++++++++++.=.==.=.++++++++++++++++++++"};

#define ADD_WATER(i, xx, yy, ww, hh) \
    l->water[i].x = xx*16; \
    l->water[i].y = yy*16; \
    l->water[i].l.w = ww*16; \
    l->water[i].l.h = hh*16;

#define ADD_LAVA(i, xx, yy, ww, hh) \
    l->lava[i].x = xx*16; \
    l->lava[i].y = yy*16; \
    l->lava[i].l.w = ww*16; \
    l->lava[i].l.h = hh*16;

#define ADD_BOULDER(i, xx, yy, ww, hh, rh) \
    l->boulder[i].x = xx*16; \
    l->boulder[i].y = yy*16; \
    l->boulder[i].original_y = yy*16; \
    l->boulder[i].w = ww*16; \
    l->boulder[i].h = hh*16; \
    l->boulder[i].range_h = rh*16; \
    l->boulder[i].shape.w = ww*16; \
    l->boulder[i].shape.h = hh*16;

void level_generate(struct level *l)
{
    int w = 64;
    int h = 64;
    
    int *map = malloc(w*h * sizeof *map);
    int *map_info = malloc(w*h * sizeof *map_info);
    memset(map_info, 0, w*h * sizeof *map_info);
    
    int *ctt = malloc(256 * sizeof(int));
    memset(ctt, 0, 256 * sizeof(int));
    ctt['.'] = T_STONE;
    ctt['g'] = T_GRASS_TOP;
    ctt['G'] = T_GRASS_BLOCK;
    ctt['m'] = T_GRASS_MIX;
    ctt['+'] = T_UNDERGROUND;
    ctt['#'] = T_LADDER;
    ctt['%'] = T_COIN;
    ctt['@'] = T_CHEST;
    ctt['&'] = T_SAND_1;
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            char c = level[y][x];
            int tile = ctt[c];
            map[y*w + x] = tile;
        }
    }
    
    free(ctt);
    
    l->map_tiles = map;
    l->map_tiles_info = map_info;
    l->width = w;
    l->height = h;
    l->spawn_x = 4*16;
    l->spawn_y = 3*16;
    
    l->water_count = 2;
    l->water = malloc(l->water_count * sizeof *l->water);
    for (int i = 0; i < l->water_count; i++) {
        l->water[i].l.r = 0;
        l->water[i].l.g = 0;
        l->water[i].l.b = 1;
        l->water[i].l.phase = 0;
    }
    
    ADD_WATER(0, 1, 46, 4, 1);
    ADD_WATER(1, 37, 47, 6, 17);
    
    l->lava_count = 4;
    l->lava  = malloc(l->lava_count * sizeof *l->lava);
    for (int i = 0; i < l->lava_count; i++) {
        l->lava[i].l.r = 1;
        l->lava[i].l.g = 0.25;
        l->lava[i].l.b = 0;
        l->lava[i].l.phase = 0;
    }
    
    ADD_LAVA(0, 51, 47, 9, 3);
    ADD_LAVA(1, 46, 37, 7, 1);
    ADD_LAVA(2, 19, 21, 10, 2);
    ADD_LAVA(3, 24, 6, 9, 1);
    
    l->boulder_count = 10;
    l->boulder = malloc(l->boulder_count * sizeof *l->boulder);
    for (int i = 0; i < l->boulder_count; i++) {
        l->boulder[i].state = BS_UNACTIVATED;
        l->boulder[i].tick = 0;
        l->boulder[i].vel = 0;
        l->boulder[i].shape.type = SHAPE_RECT;
        l->boulder[i].shape.r = 0.7;
        l->boulder[i].shape.g = 0.7;
        l->boulder[i].shape.b = 0.7;
        l->boulder[i].shape.a = 1.0;
    }
    
    ADD_BOULDER(0, 28, 38, 7, 2, 9);
    ADD_BOULDER(1, 22, 13, 2, 2, 7);
    ADD_BOULDER(2, 25, 13, 2, 2, 7);
    ADD_BOULDER(3, 41, 5, 3, 3, 15);
    ADD_BOULDER(4, 53, 21, 6, 1, 16);
    ADD_BOULDER(5, 55, 38, 2, 3, 12);
    ADD_BOULDER(6, 25, 30, 2, 2, 7);
    ADD_BOULDER(7, 28, 30, 2, 2, 7);
    ADD_BOULDER(8, 31, 30, 2, 2, 7);
    ADD_BOULDER(9, 16, -1, 5, 1, 7);
}

void level_fini(const struct level *l)
{
    free(l->boulder);
    free(l->lava);
    free(l->water);
    free(l->map_tiles_info);
    free(l->map_tiles);
}

void level_set_tile(struct level *l, int x, int y, int tile)
{
    l->map_tiles[y*l->width + x] = tile;
}

int level_get_tile_info(struct level *l, int x, int y)
{
    return l->map_tiles_info[y*l->width + x];
}

void level_set_tile_info(struct level *l, int x, int y, int info)
{
    l->map_tiles_info[y*l->width + x] = info;
}

