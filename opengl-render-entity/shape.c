#include <stdlib.h>
#include <math.h>
#include "entity/shape.h"
#include "opengl.h"

#define PI 3.14159265

RENDER_PROTO(shape)
{
    const struct entity_shape *shape = data;
    
    glColor4f(shape->r, shape->g, shape->b, shape->a);
    
    switch (shape->type) {
    case SHAPE_RECT:
        glBegin(GL_QUADS);
            glVertex2f(0,        0);
            glVertex2f(shape->w, 0);
            glVertex2f(shape->w, shape->h);
            glVertex2f(0,        shape->h);
        glEnd();
        break;
    case SHAPE_CIRCLE:
    {
        const int resolution = 30;
        
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0.0f, 0.0f);
            
            for (int i = 0; i <= resolution; i++) {
                float r = (float)i/resolution * (2*PI);
                float x, y;
                
                x = cosf(r) * shape->w;
                y = sinf(r) * shape->h;
                
                glVertex2f(x, y);
            }
        glEnd();
    }
        break;
    }
}

