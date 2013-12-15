#ifndef GAMEPLAY_STATE_H
#define GAMEPLAY_STATE_H

#include "game.h"
#include "render-list.h"

struct gameplay_state {
    size_t handle_size;
    void (*init)(void *handle, const struct game *game);
    void (*fini)(const void *handle);
    void (*step)(void *handle, const struct input_state *input,
                 struct render_list *list);
};

#define GAMEPLAY_STATE_INIT(s, name) {\
    s->handle_size = sizeof(struct name##_state);\
    s->init        = gameplay_##name##_init; \
    s->fini        = gameplay_##name##_fini; \
    s->step        = gameplay_##name##_step; \
    }

void gameplay_startscreen(struct gameplay_state *s);
void gameplay_level(struct gameplay_state *s);
void gameplay_treasure(struct gameplay_state *s);

void gameplay_test(struct gameplay_state *s);

#endif

