#include <string.h>
#include <SDL_mixer.h>
#include "opengl-render.h"
#include "asset-library.h"
#include "image.h"
#include "tileset.h"
#include "sound.h"
#include "error.h"

struct asset_library {
    struct opengl_render *gl;
};

size_t asset_library_struct_size()
{
    return sizeof(struct asset_library);
}

void asset_library_init(struct asset_library *lib, struct opengl_render *gl)
{
    lib->gl = gl;
}

void asset_library_fini(const struct asset_library *lib)
{
}

static void asset_get_full_path(const char *filename, char *out, size_t out_len)
{
    /* XXX */
    strncpy(out, "res/", out_len);
    strncat(out, filename, out_len);
}

int asset_library_load_image(struct asset_library *lib,
                             const char *filename, struct image *image)
{
    int err;
    char buf[256];
    asset_get_full_path(filename, buf, sizeof(buf));
    
    err = opengl_load_image(lib->gl, buf, image);
    if (err) {
        error_out("Could not load image");
        error_out(filename);
    }
    
    return err;
}

int asset_library_load_tileset(
                             struct asset_library *lib,
                             const char *filename, int tile_w, int tile_h,
                             struct tileset *tileset)
{
    int err = 0;
    
    err = asset_library_load_image(lib, filename, &tileset->image);
    if (err) {
        goto error_asset;
    }

    tileset->tile_w = tile_w;
    tileset->tile_h = tile_h;
    
error_asset:
    return err;
}

int asset_library_load_sound(struct asset_library *lib,
                             const char *filename, struct sound *sound)
{
    int err = 0;
    char buf[256];
    asset_get_full_path(filename, buf, sizeof(buf));
    
    if (!(sound->chunk = Mix_LoadWAV(buf))) {
        error_out("Could not load sound");
        error_out(filename);
        error_out(Mix_GetError());
        err = -1;
        goto error_asset;
    }
    
error_asset:
    return err;
}

