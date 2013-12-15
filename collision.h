#ifndef COLLISION_H
#define COLLISION_H

struct collision_box {
    float x, y, w, h;
};

int collision_test_box(const struct collision_box *box, float x, float y);
int collision_test_box_box(const struct collision_box *box_a,
                           const struct collision_box *box_b);
int collision_test_complex(const struct collision_box *box,
                           int (*points)(const void *handle, float *x, float *y, int off),
                           const void *handle);

int collision_test_circle(const struct collision_box *circle, float x, float y);
int collision_test_circle_box(const struct collision_box *circle,
                              const struct collision_box *box);

#endif

