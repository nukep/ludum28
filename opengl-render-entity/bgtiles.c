#include "entity/bgtiles.h"
#include "opengl.h"

RENDER_PROTO(bgtiles)
{
    const struct entity_bgtiles *bgtiles = data;
    
    bind_image(&bgtiles->tileset->image);
    
    for (int y = 0; y < bgtiles->h; y++) {
        for (int x = 0; x < bgtiles->w; x++) {
            int tile;
            
            tile = bgtiles->get_tile(bgtiles->handle, x, y);
            
            if (tile != 0) {
                glPushMatrix();
                glTranslatef(x*bgtiles->tileset->tile_w, y*bgtiles->tileset->tile_h, 0);
                draw_tile(bgtiles->tileset, tile);
                glPopMatrix();
            }
        }
    }
}

