#include <math.h>
#include "entity/liquid.h"
#include "opengl.h"

RENDER_PROTO(liquid)
{
    const struct entity_liquid *liquid = data;
    
    const int resolution = liquid->w/2;
    const float amp = 4;
    
    glColor4f(liquid->r, liquid->g, liquid->b, 0.5);
    
    glBegin(GL_TRIANGLE_FAN);
        /* bottom center */
        glVertex2f(liquid->w/2, liquid->h);
        
        /* bottom left */
        glVertex2f(0, liquid->h);
        
        /* wavy top */
        for (int i = 0; i <= resolution; i++) {
            float b = (sinf(((((float)i/resolution))*16 + liquid->phase)/4)+1.0f)/2.0f;
            b = b * amp;
            
            glVertex2f(((float)i/resolution)*liquid->w, b);
        }
        
        /* bottom right*/
        glVertex2f(liquid->w, liquid->h);
    glEnd();
}

