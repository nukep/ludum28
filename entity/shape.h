#ifndef ENTITY_SHAPE_H
#define ENTITY_SHAPE_H

enum entity_shape_type {
    SHAPE_RECT, SHAPE_CIRCLE
};

struct entity_shape {
    enum entity_shape_type type;
    int w, h;
    float r, g, b, a;
};

#endif

