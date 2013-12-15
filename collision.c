#include <math.h>
#include "collision.h"

int collision_test_box(const struct collision_box *box, float x, float y) {
    return x >= box->x && x < box->x+box->w &&
           y >= box->y && y < box->y+box->h;
}

int collision_test_box_box(const struct collision_box *box_a,
                           const struct collision_box *box_b)
{
    int ret = 0;
    ret |= collision_test_box(box_b, box_a->x,          box_a->y);
    ret |= collision_test_box(box_b, box_a->x+box_a->w, box_a->y);
    ret |= collision_test_box(box_b, box_a->x+box_a->w, box_a->y+box_a->h);
    ret |= collision_test_box(box_b, box_a->x,          box_a->y+box_a->h);
    
    ret |= collision_test_box(box_a, box_b->x,          box_b->y);
    ret |= collision_test_box(box_a, box_b->x+box_b->w, box_b->y);
    ret |= collision_test_box(box_a, box_b->x+box_b->w, box_b->y+box_b->h);
    ret |= collision_test_box(box_a, box_b->x,          box_b->y+box_b->h);
    
    return ret;
}

int collision_test_complex(const struct collision_box *box,
                           int (*points)(const void *handle, float *x, float *y, int off),
                           const void *handle)
{
    float x, y;
    int off = 0;
    while (points(handle, &x, &y, off)) {
        if (collision_test_box(box, x, y))
        {
            return 1;
        }
        off++;
    }
    return 0;
}

int collision_test_circle(const struct collision_box *circle, float x, float y)
{
    float cx = circle->x + circle->w/2.0;
    float cy = circle->y + circle->h/2.0;
    
    float norm_x, norm_y;
    /* this allows us to check for a radius of 0.5, for each axis */
    norm_x = cx + (x-cx)/circle->w;
    norm_y = cy + (y-cy)/circle->h;
    
    float dist = hypot(cx-norm_x, cy-norm_y);
    
    return dist <= 0.5;
}

int collision_test_circle_box(const struct collision_box *c,
                              const struct collision_box *b)
{
    int ret = 0;
    ret |= collision_test_circle(c, b->x,      b->y);
    ret |= collision_test_circle(c, b->x+b->w, b->y);
    ret |= collision_test_circle(c, b->x+b->w, b->y+b->h);
    ret |= collision_test_circle(c, b->x,      b->y+b->h);
    
    return ret;
}

