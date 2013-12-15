#ifndef RENDER_LIST_H
#define RENDER_LIST_H

struct render_entity_attr {
    float origin_x, origin_y;
    float scale_x, scale_y;
    float rotate;
    float x, y;
};

struct render_entity {
    int type;
    const void *data;
    struct render_entity_attr attr;
};

struct render_stage {
    int camera_x, camera_y;
    float fade;
    float fade_r, fade_g, fade_b;
    float bg_r, bg_g, bg_b;
};

struct render_list {
    struct render_stage stage;
    int count;
    int capacity;
    struct render_entity *entities;
};

void render_list_init(struct render_list *list);
void render_list_fini(const struct render_list *list);

void render_list_clear(struct render_list *list);
void render_list_add_rxy(struct render_list *list, int type, const void *data,
                         float rotate, float x, float y);
void render_list_add_orxy(
                         struct render_list *list, int type, const void *data,
                         float origin_x, float origin_y,
                         float rotate, float x, float y);
void render_list_add_osrxy(
                         struct render_list *list, int type, const void *data,
                         float origin_x, float origin_y,
                         float scale_x, float scale_y,
                         float rotate, float x, float y);

void render_list_set_camera(struct render_list *list, int x, int y);
void render_list_set_bg(struct render_list *list, float r, float g, float b);

#endif

