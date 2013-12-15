#include <math.h>
#include "opengl.h"
#include "entity/blob2.h"

#define PI 3.14159265
static const float CENTER_OFF = 0.95f;

struct point {
    float x, y;
};

static float lerp(float a, float b, float p)
{
    return (b-a)*p + a;
}

static float ease(float x, float s)
{
    const float nx = (x + 1)/2;
    
    return (powf(nx, 1.0f-s))*2 - 1;
}


static void transform_rotate(float r, float x, float y, float *ox, float *oy)
{
    const float cr = cosf(r);
    const float sr = sinf(r);
    
    *ox = cr*x - sr*y;
    *oy = sr*x + cr*y;
}

static void transform_squish(float s, float x, float y, float *ox, float *oy)
{
    const float nx = (x + 1)/2;
    
    const float off_s = 1.0f+s*0.2f;
    
    *ox = ease(lerp(-1.0f+s*0.2f, 1.0f, nx), s);
    *oy = y;
}

static void transform_stretch(float s, float x, float y, float *ox, float *oy)
{
    const float nx = (x + 1)/2;
    
    *ox = lerp(-1.0f - s, 1.0f, nx);
    *oy = y * (1-s);
}

static void get_points(const struct entity_blob2 *blob, struct point *points, int resolution)
{
    for (int i = 0; i <= resolution; i++) {
        float r = (float)i/resolution * (2*PI);
        float x, y;
        int squishing = blob->squish > 0;
        
        x = cosf(r);
        y = sinf(r);
        
        transform_rotate(-blob->angle, x, y, &x, &y);
        
        if (squishing) {
            transform_squish(blob->squish<CENTER_OFF?blob->squish:CENTER_OFF, x, y, &x, &y);
        } else {
            transform_stretch(-blob->squish, x, y, &x, &y);
        }
        
        transform_rotate(blob->angle, x, y, &x, &y);
        
        if (squishing) {
            if (x < -1) x = -1;
            if (y < -1) y = -1;
            if (x > 1) x = 1;
            if (y > 1) y = 1;
        }
        
        x = lerp(-0.25f, 1.0f, x);
        
        points[i].x = x;
        points[i].y = y;
    }
}

RENDER_PROTO(blob2)
{
    const struct entity_blob2 *blob = data;
    const int resolution = 50;
    struct point points[resolution+1];
    
    get_points(blob, points, resolution);
    
    /* fill */
    glColor3f(blob->r, blob->g, blob->b);

    glBegin(GL_TRIANGLE_FAN);
    {
        float center_x, center_y;
        transform_rotate(blob->angle, CENTER_OFF, 0, &center_x, &center_y);
        glVertex2f(center_x, center_y);
        
        for (int i = 0; i <= resolution; i++) {
            glVertex2f(points[i].x, points[i].y);
        }
    }
    glEnd();

    /* outline */
    const float thick = 0.025;
    const float dark = 0.5;
    
    glColor3f(blob->r*dark, blob->g*dark, blob->b*dark);
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= resolution; i++) {
            glVertex2f(points[i].x*(1-thick), points[i].y*(1-thick));
            glVertex2f(points[i].x*(1+thick), points[i].y*(1+thick));
        }
    glEnd();
}

