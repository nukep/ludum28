#include "state.h"
#include "entities.h"
#include "tiles.h"

struct treasure_state {
    const struct game     *game;
    struct entity_text    info_text;
    struct entity_sprite  coin_sprite;
    int entered;
};

static void gameplay_treasure_init(void *handle, const struct game *game)
{
    struct treasure_state *s = handle;
    s->game = game;
    
    s->info_text.font = &game->main_font;
    s->info_text.text = "You found...\n\n\nA single coin!\n\nYou've earned it!";
    
    s->coin_sprite.tileset = &game->tileset;
    s->coin_sprite.tile = T_COIN;
    
    s->entered = 0;
}

static void gameplay_treasure_fini(const void *handle)
{
}

static void gameplay_treasure_step(void *handle,
        const struct input_state *input, struct render_list *list)
{
    struct treasure_state *s = handle;

    render_list_add_rxy(list, ENTITY_text, &s->info_text, 0, 36, 50);
    render_list_add_rxy(list, ENTITY_sprite, &s->coin_sprite, 0, 40, 100);
    
    render_list_set_bg(list, 0, 0, 0);
    
    if (!s->entered && input->any_key) {
        /* any key has been pressed! */
        struct gameplay_state st;
        gameplay_startscreen(&st);
        game_start_state_transition(s->game, &st);
        
        s->entered = 1;
        game_play_sound(&s->game->start);
    }
}

void gameplay_treasure(struct gameplay_state *s)
{
    GAMEPLAY_STATE_INIT(s, treasure);
}

