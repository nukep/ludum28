#ifndef GAMEPLAY_OBJECT_H
#define GAMEPLAY_OBJECT_H

struct collision_box;

struct object {
    float vel_x, vel_y;
    float pos_x, pos_y;
    int bound_w, bound_h;
    
    float friction;
    float perp_friction;
    int circle;
    int permeability;
    float gravity;
};

static inline void object_init(struct object *obj, float pos_x, float pos_y,
                               int bound_w, int bound_h, float friction)
{
    obj->vel_x = 0;
    obj->vel_y = 0;
    obj->pos_x = pos_x;
    obj->pos_y = pos_y;
    obj->bound_w = bound_w;
    obj->bound_h = bound_h;
    obj->friction = friction;
    obj->perp_friction = 0.1f;
    obj->circle = 0;
    obj->permeability = 1;
    obj->gravity = 1.0f;
}

int object_step(struct object *obj,
                 int get_tile_solid(const void *field_handle, int x, int y),
                 const void *field_handle);

void object_get_collision_box(const struct object *obj,
                              struct collision_box *box);

int objects_collide(const struct object *a, const struct object *b);

#endif

