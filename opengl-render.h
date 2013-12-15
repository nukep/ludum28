#ifndef OPENGL_RENDER_H
#define OPENGL_RENDER_H

#include <stddef.h>

struct opengl_render;
struct render_list;
struct image;

typedef void (*opengl_render_entity_t)(const void *data);

size_t opengl_render_struct_size();
void opengl_render_init(struct opengl_render *gl);
void opengl_render_fini(const struct opengl_render *gl);

void opengl_render_add_type(struct opengl_render *gl,
                            int type, opengl_render_entity_t r);

int opengl_load_image(struct opengl_render *gl, const char *filename,
                      struct image *image);

void opengl_render(const struct opengl_render *gl,
                   const struct render_list *list, int w, int h);

#endif

