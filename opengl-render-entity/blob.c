#include <math.h>
#include "opengl.h"
#include "entity/blob.h"

#define PI 3.14159265

RENDER_PROTO(blob)
{
    const int resolution = 30;
    
    const struct entity_blob *blob = data;
    
    /* as speed increases, x stretches and y narrows */
    
    double x_stretch = 1+(blob->speed/16);
    double y_stretch = 1/x_stretch;
    
    glColor4f(blob->r, blob->g, blob->b, 0.75);
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        
        float radius = 1.0;
        
        for (int i = 0; i <= resolution; i++) {
            float r = (float)i/resolution * (2*PI);
            float x, y;
            
            x = cosf(r) * radius*x_stretch;
            y = sinf(r) * radius*y_stretch;
            
            glVertex2f(x, y);
        }
    
    glEnd();
}

