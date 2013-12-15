#include "entity/sprite.h"
#include "opengl.h"

RENDER_PROTO(sprite)
{
    const struct entity_sprite *sprite = data;
    
    bind_image(&sprite->tileset->image);
    draw_tile(sprite->tileset, sprite->tile);
}

