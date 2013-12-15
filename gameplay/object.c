#include <math.h>
#include "game.h"
#include "object.h"
#include "collision.h"

static const int tile_size = 16;
static const int iterations = 64;
static const float GRAVITY_TERMINAL = 256;
static const float GRAVITY_INC = 8;

static int object_collision_test(struct object *obj,
                 int get_tile_solid(const void *field_handle, int x, int y),
                 const void *field_handle)
{
    struct collision_box obj_box;
    int xfrom, xto, yfrom, yto;
    
    object_get_collision_box(obj, &obj_box);
    
    xfrom = (int)floor(obj->pos_x/tile_size);
    xto   = (int)ceil((obj->pos_x+obj->bound_w)/tile_size);
    yfrom = (int)floor((obj->pos_y)/tile_size);
    yto   = (int)ceil((obj->pos_y+obj->bound_h)/tile_size);
    
    for (int y = yfrom; y <= yto; y++) {
        for (int x = xfrom; x <= xto; x++) {
            int tile = get_tile_solid(field_handle, x, y) >= obj->permeability;
            
            if (tile) {
                /* tile present, collision detection active */
                struct collision_box tile_box =
                    {.x = x*tile_size, .y = y*tile_size,
                     .w = tile_size,   .h = tile_size};
                
                if (!obj->circle) {
                    if (collision_test_box_box(&obj_box, &tile_box)) {
                        /* collision */
                        return 1;
                    }
                } else {
                    if (collision_test_circle_box(&obj_box, &tile_box)) {
                        /* collision */
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

static int object_collision_adjust(struct object *obj,
                 int get_tile_solid(const void *field_handle, int x, int y),
                 const void *field_handle)
{
    /* if there's a collision, try adjusting from the opposite direction of
     * the velocity */
    const float x_old = obj->pos_x;
    const float y_old = obj->pos_y;
    const float x_new = obj->pos_x - obj->vel_x/(FPS*iterations);
    const float y_new = obj->pos_y - obj->vel_y/(FPS*iterations);
    
    obj->pos_x = x_new;
    obj->pos_y = y_old;
    
    if (!object_collision_test(obj, get_tile_solid, field_handle)) {
        /* adjusting x worked! */
        obj->vel_x = -obj->vel_x*obj->friction;
        obj->vel_y *= 1-((1-obj->friction)*obj->perp_friction);
        return 0;
    }
    
    obj->pos_x = x_old;
    obj->pos_y = y_new;
    
    if (!object_collision_test(obj, get_tile_solid, field_handle)) {
        /* adjusting y worked! */
        obj->vel_x *= 1-((1-obj->friction)*obj->perp_friction);
        obj->vel_y = -obj->vel_y*obj->friction;
        return 0;
    }
    
    obj->pos_x = x_new;
    obj->pos_y = y_new;
    
    if (!object_collision_test(obj, get_tile_solid, field_handle)) {
        /* adjusting both worked! */
        obj->vel_x = -obj->vel_x*obj->friction;
        obj->vel_y = -obj->vel_y*obj->friction;
        return 0;
    }
    
    obj->pos_x = x_old;
    obj->pos_y = y_old;
    
    return -1;
}

static int dummy_get_tile_solid(const void *field_handle, int x, int y)
{
    return 0;
}

int object_step(struct object *obj,
                 int get_tile_solid(const void *field_handle, int x, int y),
                 const void *field_handle)
{
    int collided = 0;
    if (!get_tile_solid) {
        get_tile_solid = dummy_get_tile_solid;
    }
    
    for (int i = 0; i < iterations; i++) {
        obj->pos_x += obj->vel_x/(FPS*iterations);
        obj->pos_y += obj->vel_y/(FPS*iterations);
        
        if (object_collision_test(obj, get_tile_solid, field_handle)) {
            /* adjust */
            int unchanged = object_collision_adjust(obj, get_tile_solid, field_handle);
            
            collided = 1;
            
            if (unchanged) {
                /* adjustments didn't work, keep as is */
                return 0;
            }
        }
    }
    
    /* gravity */
    if (obj->vel_y < GRAVITY_TERMINAL*obj->gravity) {
        obj->vel_y += GRAVITY_INC*obj->gravity;
    } else {
        obj->vel_y = GRAVITY_TERMINAL*obj->gravity;
    }
    
    if (fabsf(obj->vel_x) < 1) {
        obj->vel_x = 0;
    }
    
    if (fabsf(obj->vel_y) < 1) {
        obj->vel_y = 0;
    }
    
    return collided;
}

void object_get_collision_box(const struct object *a,
                              struct collision_box *box)
{
    *box = (struct collision_box)
           {.x = a->pos_x,   .y = a->pos_y,
            .w = a->bound_w, .h = a->bound_h};
}

int objects_collide(const struct object *a, const struct object *b)
{
    struct collision_box box_a, box_b;
    
    object_get_collision_box(a, &box_a);
    object_get_collision_box(b, &box_b);
    
    return collision_test_box_box(&box_a, &box_b);
}

