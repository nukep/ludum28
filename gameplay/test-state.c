#include "state.h"
#include "entities.h"

struct test_state {
    const struct game *game;
    
    struct entity_blob2 blob2;
    struct entity_shape eye, eye2;
};

static void gameplay_test_init(void *handle, const struct game *game)
{
    struct test_state *s = handle;
    s->game = game;
    
    s->blob2 = (struct entity_blob2){.squish = -0.5f, .angle=0.0f, .r=0.4, .g=0.4, .b=1};
    s->eye = (struct entity_shape){.type = SHAPE_CIRCLE, .w=10, .h=10, .r=0, .g=0, .b=0, .a=1};
    s->eye2 = (struct entity_shape){.type = SHAPE_CIRCLE, .w=5, .h=5, .r=1, .g=1, .b=1, .a=1};
}

static void gameplay_test_fini(const void *handle)
{
}

static void gameplay_test_step(void *handle,
        const struct input_state *input, struct render_list *list)
{
    struct test_state *s = handle;
    
    render_list_add_osrxy(list, ENTITY_blob2, &s->blob2, 0, 0, 50, 50, 0, 100, 100);
    render_list_add_rxy(list, ENTITY_shape, &s->eye, 0, 120, 90);
    render_list_add_rxy(list, ENTITY_shape, &s->eye2, 0, 117, 87);
    
    s->blob2.squish += 0.005;
    //s->blob2.angle += 0.01;
}

void gameplay_test(struct gameplay_state *s)
{
    GAMEPLAY_STATE_INIT(s, test);
}

