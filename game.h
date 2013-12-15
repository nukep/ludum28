#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include "entities.h"
#include "sound.h"

static const int FPS = 60;

struct game_priv;

struct game {
    struct tileset main_font;
    struct tileset tileset;
    
    struct sound start, coin, death, jump, key_ding, key_pickup, splash,
                 boulder_budge, boulder_down;
    
    struct game_priv *priv;
};

struct render_list;
struct asset_library;
struct gameplay_state;

struct input_state {
    /* x/y ranges from -1 to +1 */
    int x;
    int y;
    
    /* zero on release, non-zero on press */
    int action_key;
    int back_key;
    int any_key;
};

int game_init(struct game *game);
void game_fini(const struct game *game);

int game_load_assets(struct game *game, struct asset_library *lib);

void game_step(struct game *game, const struct input_state *input,
               struct render_list *list);

void game_start_state_transition(const struct game *game,
                                 const struct gameplay_state *state);

void game_play_sound(const struct sound *sfx);

#endif

