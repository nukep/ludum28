#include <stdlib.h>
#include <assert.h>
#include "render-list.h"

void render_list_init(struct render_list *list)
{
    list->stage.camera_x = 0;
    list->stage.camera_y = 0;
    list->stage.fade     = 0;
    list->stage.fade_r   = 0;
    list->stage.fade_g   = 0;
    list->stage.fade_b   = 0;
    list->stage.bg_r   = 0;
    list->stage.bg_g   = 0;
    list->stage.bg_b   = 0;
    list->count = 0;
    list->capacity = 256;
    
    list->entities = malloc(list->capacity * sizeof *list->entities);
}

void render_list_fini(const struct render_list *list)
{
    free(list->entities);
}

void render_list_clear(struct render_list *list)
{
    list->count = 0;
}


void render_list_add_osrxy(
                         struct render_list *list, int type, const void *data,
                         float origin_x, float origin_y,
                         float scale_x, float scale_y,
                         float rotate, float x, float y)
{
    struct render_entity e;
    e.type = type;
    e.data = data;
    e.attr.x = x;
    e.attr.y = y;
    e.attr.scale_x = scale_x;
    e.attr.scale_y = scale_y;
    e.attr.rotate = rotate;
    e.attr.origin_x = origin_x;
    e.attr.origin_y = origin_y;
    
    list->entities[list->count] = e;
    list->count++;
    
    assert(list->count <= list->capacity);
}

void render_list_add_orxy(
                         struct render_list *list, int type, const void *data,
                         float origin_x, float origin_y,
                         float rotate, float x, float y)
{
    render_list_add_osrxy(list, type, data, origin_x, origin_y, 1, 1, rotate, x, y);
}

void render_list_add_rxy(struct render_list *list, int type, const void *data,
                         float rotate, float x, float y)
{
    render_list_add_orxy(list, type, data, 0, 0, rotate, x, y);
}

void render_list_set_camera(struct render_list *list, int x, int y)
{
    list->stage.camera_x = x;
    list->stage.camera_y = y;
}

void render_list_set_bg(struct render_list *list, float r, float g, float b)
{
    list->stage.bg_r = r;
    list->stage.bg_g = g;
    list->stage.bg_b = b;
}

