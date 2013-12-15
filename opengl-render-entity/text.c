#include <math.h>
#include "opengl.h"
#include "entity/text.h"

static int char_to_offset(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return c-'A';
    } else if (c >= 'a' && c <= 'z') {
        return c-'a' + 32;
    } else if (c >= '0' && c <= '9') {
        return c-'0' + 48;
    } else {
        switch (c) {
        case '!': return 26;
        case '.': return 27;
        case '?': return 28;
        case '#': return 29;
        case ',': return 30;
        case '\'': return 31;
        }
        
        /* no character available */
        return -1;
    }
}

RENDER_PROTO(text)
{
    const struct entity_text *text = data;
    
    const char *c = text->text;
    
    int x, y;
    
    x = 0;
    y = 0;
    bind_image(&text->font->image);
    
    while (*c) {
        if (*c == '\n') {
            /* newline */
            x = 0;
            y++;
        } else if (*c == ' ') {
            x++;
        } else {
            const int off = char_to_offset(*c);
            
            glPushMatrix();
            glTranslatef(x*text->font->tile_w, y*text->font->tile_h, 0);
            draw_tile(text->font, off);
            glPopMatrix();
            x++;
        }
        
        c++;
    }
}

