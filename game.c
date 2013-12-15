#include <stdlib.h>
#include <SDL_mixer.h>
#include "error.h"
#include "game.h"
#include "gameplay/state.h"
#include "render-list.h"
#include "asset-library.h"
#include "tileset.h"

struct game_priv {
    struct gameplay_state gameplay_state;
    void *gameplay_state_data;
    
    int transitioning;
    int transition_tick;
    struct gameplay_state pending_gameplay_state;
};

static void game_fini_state(const struct game *game)
{
    if (game->priv->gameplay_state_data) {
        game->priv->gameplay_state.fini(game->priv->gameplay_state_data);
        free(game->priv->gameplay_state_data);
    }
}

static void game_init_state(struct game *game, const struct gameplay_state *s)
{
    game_fini_state(game);
    game->priv->gameplay_state = *s;
    
    game->priv->gameplay_state_data = malloc(game->priv->gameplay_state.handle_size);
    game->priv->gameplay_state.init(game->priv->gameplay_state_data, game);
}

int game_init(struct game *game)
{
    game->priv = malloc(sizeof *game->priv);
    
    gameplay_startscreen(&game->priv->gameplay_state);
    game->priv->gameplay_state_data = 0;
    game_init_state(game, &game->priv->gameplay_state);
    
    game->priv->transitioning = 0;
    
    return 0;
}

void game_fini(const struct game *game)
{
    game_fini_state(game);
    free(game->priv);
}

int game_load_assets(struct game *game, struct asset_library *lib)
{
    #define SOUND(x) { \
        err = asset_library_load_sound(lib, #x ".wav", &game->x); \
        if (err) { \
            goto error_assets; \
        } \
    }
    int err = 0;
    
    err = asset_library_load_tileset(lib, "font.png", 8, 8, &game->main_font);
    if (err) {
        goto error_assets;
    }
    
    err = asset_library_load_tileset(lib, "tileset.png", 16, 16, &game->tileset);
    if (err) {
        goto error_assets;
    }
    
    SOUND(start);
    SOUND(coin);
    SOUND(death);
    SOUND(jump);
    SOUND(key_ding);
    SOUND(key_pickup);
    SOUND(splash);
    SOUND(boulder_budge);
    SOUND(boulder_down);
    
error_assets:
    return err;
}

void game_step(struct game *game, const struct input_state *input,
               struct render_list *list)
{
    static const int TRANSITION_TICKS = 30;
    
    if (game->priv->transitioning == +1) {
        /* fade out */
        game->priv->transition_tick++;
        list->stage.fade = (float)game->priv->transition_tick/TRANSITION_TICKS;
        if (game->priv->transition_tick == TRANSITION_TICKS) {
            /* opaque - replace state */
            game->priv->transitioning = -1;
            render_list_set_camera(list, 0, 0);
            game_init_state(game, &game->priv->pending_gameplay_state);
        }
    } else if (game->priv->transitioning == -1) {
        /* fade in */
        game->priv->transition_tick--;
        list->stage.fade = (float)game->priv->transition_tick/TRANSITION_TICKS;
        if (game->priv->transition_tick == 0) {
            /* finished */
            game->priv->transitioning = 0;
        }
    }
    
    game->priv->gameplay_state.step(game->priv->gameplay_state_data, input, list);
}

void game_start_state_transition(const struct game *game,
                                 const struct gameplay_state *state)
{
    game->priv->transitioning = 1;
    game->priv->transition_tick = 0;
    game->priv->pending_gameplay_state = *state;
}

void game_play_sound(const struct sound *sfx)
{
    int channel;
    channel = Mix_PlayChannel(-1, sfx->chunk, 0);
    if (channel) {
        error_out("Could not play sound");
        error_out(Mix_GetError());
    }
}

