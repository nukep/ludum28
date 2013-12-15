#ifndef ASSET_LIBRARY_H
#define ASSET_LIBRARY_H

#include <stddef.h>

struct asset_library;
struct opengl_render;
struct image;
struct font;
struct tileset;
struct sound;

size_t asset_library_struct_size();
void asset_library_init(struct asset_library *lib, struct opengl_render *gl);
void asset_library_fini(const struct asset_library *lib);

int asset_library_load_image(struct asset_library *lib,
                             const char *filename, struct image *image);
int asset_library_load_tileset(
                             struct asset_library *lib,
                             const char *filename, int tile_w, int tile_h,
                             struct tileset *tileset);
int asset_library_load_sound(struct asset_library *lib,
                             const char *filename, struct sound *sound);

#endif

