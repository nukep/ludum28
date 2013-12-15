#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <SDL2/SDL_image.h>
#include "render-list.h"
#include "opengl-render.h"
#include "image.h"

static const int MAX_RENDER_CALLBACKS = 256;

struct opengl_render {
    opengl_render_entity_t *render_callbacks;
};

size_t opengl_render_struct_size()
{
    return sizeof(struct opengl_render);
}

void opengl_render_init(struct opengl_render *gl)
{
    const size_t sz = MAX_RENDER_CALLBACKS * sizeof *gl->render_callbacks;
    gl->render_callbacks = malloc(sz);
    
    /* Initialize all to null */
    memset(gl->render_callbacks, 0, sz);
}

void opengl_render_fini(const struct opengl_render *gl)
{
    free(gl->render_callbacks);
}

void opengl_render_add_type(struct opengl_render *gl,
                            int type, opengl_render_entity_t r)
{
    assert(type >= 0 && type < MAX_RENDER_CALLBACKS);
    
    gl->render_callbacks[type] = r;
}

int opengl_load_image(struct opengl_render *gl, const char *filename,
                      struct image *image)
{
    int ret = 0;
    SDL_Surface *surface = IMG_Load(filename);
    
    if (!surface) {
        ret = -1;
        goto error_surface;
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    
    glBindTexture(GL_TEXTURE_2D, tex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);

    image->asset_id = (int)tex;
    image->w = surface->w;
    image->h = surface->h;
    
    SDL_FreeSurface(surface);

    return ret;

error_surface:
    return -1;
}

void opengl_render(const struct opengl_render *gl,
                   const struct render_list *list, int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(2,2,2);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(list->stage.bg_r, list->stage.bg_g, list->stage.bg_b, 1);
    
    glTranslatef(-list->stage.camera_x, -list->stage.camera_y, 0);
    
    glColor3f(1, 1, 1);
    
    for (int i = 0; i < list->count; i++) {
        const struct render_entity *e = list->entities + i;
        assert(e->type >= 0 && e->type < MAX_RENDER_CALLBACKS);
        
        glPushMatrix();
        glTranslatef(e->attr.x, e->attr.y, 0);
        glRotatef(e->attr.rotate, 0, 0, 1);
        glScalef(e->attr.scale_x, e->attr.scale_y, 1);
        glTranslatef(-e->attr.origin_x, -e->attr.origin_y, 0);
        
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        gl->render_callbacks[e->type](e->data);
        glPopAttrib();
        
        glPopMatrix();
    }
    
    if (list->stage.fade > 0) {
        glLoadIdentity();
        glColor4f(list->stage.fade_r, list->stage.fade_g, list->stage.fade_b,
                  list->stage.fade);
        glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(w, 0);
            glVertex2f(w, h);
            glVertex2f(0, h);
        glEnd();
    }
}

