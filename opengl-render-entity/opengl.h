#ifndef RENDER_ENTITIES_OPENGL
#define RENDER_ENTITIES_OPENGL

#include <GL/glew.h>

struct opengl_render;
struct image;
struct tileset;

void render_entity_opengl_add_all(struct opengl_render *gl);
void bind_image(const struct image *image);
void draw_tile(const struct tileset *tileset, int tile);

#define RENDER_PROTO(x) void render_entity_opengl_##x(const void *data)

#endif

