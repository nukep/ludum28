#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "error.h"
#include "timer.h"
#include "game.h"
#include "opengl-render.h"
#include "render-list.h"
#include "opengl-render-entity/opengl.h"
#include "image.h"
#include "asset-library.h"

struct display {
    SDL_Window *window;
    int w;
    int h;
};

static int audio_init(struct display *dp)
{
    int err;
    int audio_rate = 44100;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;
    
    err = Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
    
    if (err) {
        error_out(Mix_GetError());
        goto error;
    }
error:
    return err;
}

static int display_init(struct display *dp)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        error_out(SDL_GetError());
        return 1;
    }
    
    dp->w = 512;
    dp->h = 512;
    
    dp->window = SDL_CreateWindow("Blobby",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    dp->w,
                                    dp->h,
                                    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    
    SDL_GL_CreateContext(dp->window);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        error_out((const char*)glewGetErrorString(err));
    }
    
    audio_init(dp);
    
    return 0;
}

static void display_fini(const struct display *dp)
{
    SDL_DestroyWindow(dp->window);
    SDL_Quit();
}

static int game_loop(struct game *game, struct display *dp,
                     struct opengl_render *gl,
                     struct render_list *render_list)
{
    int done = 0;
    int pressed = 0;
    struct timer timer;
    timer_init(&timer);
    
    while (!done) {
        struct input_state input;
        int elapsed_ms = timer_elapsed_ms(&timer);
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                done = 1;
                break;
            case SDL_KEYDOWN:
                pressed = 1;
                break;
            case SDL_KEYUP:
                pressed = 0;
                break;
            }
        }
        
        const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
        if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP]) {
            input.y = -1;
        } else if (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN]) {
            input.y = +1;
        } else {
            input.y = 0;
        }
        
        if (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT]) {
            input.x = -1;
        } else if (keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT]) {
            input.x = +1;
        } else {
            input.x = 0;
        }
        
        input.action_key = keyboard[SDL_SCANCODE_SPACE];
        input.back_key = keyboard[SDL_SCANCODE_ESCAPE] ||
                         keyboard[SDL_SCANCODE_BACKSPACE];
        input.any_key = pressed;
        
        if (elapsed_ms*FPS >= 1000) {
            timer_reset(&timer);
            render_list_clear(render_list);
            game_step(game, &input, render_list);
        }
        opengl_render(gl, render_list, dp->w, dp->h);
        
        SDL_GL_SwapWindow(dp->window);
    }
    
    return 0;
}

static int game_run(struct game *game, struct display *dp)
{
    int err = 0;
    struct opengl_render *gl;
    struct asset_library *lib;
    struct render_list render_list;
    
    gl = malloc(opengl_render_struct_size());
    opengl_render_init(gl);
    
    lib = malloc(asset_library_struct_size());
    asset_library_init(lib, gl);
    
    render_entity_opengl_add_all(gl);
    
    err = game_load_assets(game, lib);
    if (err) {
        error_out("Could not load assets");
        goto error_assets;
    }
    
    render_list_init(&render_list);
    
    game_loop(game, dp, gl, &render_list);
    
    render_list_fini(&render_list);
    
error_assets:
    asset_library_fini(lib);
    free(lib);
    
    opengl_render_fini(gl);
    free(gl);
    
    return err;
}

int main(int argc, char *argv[])
{
    int err = 0;
    
    struct display dp;
    struct game *game;
    
    err = display_init(&dp);
    if (err) {
        goto error_display_init;
    }
    
    game = malloc(sizeof *game);
    err = game_init(game);
    if (err) {
        goto error_game_init;
    }
    
    game_run(game, &dp);
    
    game_fini(game);
    
error_game_init:
    free(game);
    display_fini(&dp);

error_display_init:
    
    return err;
}

