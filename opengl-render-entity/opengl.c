#include "opengl.h"
#include "opengl-render.h"
#include "entity/types.h"
#include "image.h"
#include "tileset.h"

#define ADD(x) RENDER_PROTO(x); opengl_render_add_type(gl, ENTITY_##x, render_entity_opengl_##x)

void render_entity_opengl_add_all(struct opengl_render *gl)
{
    ADD(blob);
    ADD(blob2);
    ADD(text);
    ADD(sprite);
    ADD(bgtiles);
    ADD(liquid);
    ADD(shape);
}

void bind_image(const struct image *image)
{
    glEnable(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, image->asset_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void draw_tile(const struct tileset *tileset, int tile)
{
    const float img_w = tileset->image.w;
    const float img_h = tileset->image.h;
    const int cw = tileset->tile_w;
    const int ch = tileset->tile_h;
    const int img_tilew = tileset->image.w/cw;
    const int cx = (tile%img_tilew)*cw;
    const int cy = (tile/img_tilew)*ch;
    
    glBegin(GL_QUADS);
        glTexCoord2f(cx/img_w, cy/img_h);
        glVertex2f(0, 0);
        
        glTexCoord2f((cx+cw)/img_w, (cy)/img_h);
        glVertex2f(cw, 0);
        
        glTexCoord2f((cx+cw)/img_w, (cy+ch)/img_h);
        glVertex2f(cw, ch);
        
        glTexCoord2f((cx)/img_w, (cy+ch)/img_h);
        glVertex2f(0, ch);
    glEnd();
}

