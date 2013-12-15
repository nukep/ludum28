#include "state.h"
#include "entities.h"

struct startscreen_state {
    const struct game *game;
    
    int entered;
    struct entity_text info_text;
    struct entity_text menu_text;
    int flash_timer;
};

static void gameplay_startscreen_init(void *handle, const struct game *game)
{
    struct startscreen_state *s = handle;
    s->game = game;
    s->menu_text.font = &game->main_font;
    s->menu_text.text = "Press any key to start!";
    
    s->info_text.font = &game->main_font;
    s->info_text.text = "You have one key.\nDON'T lose it!\n\n"
                        "Use it to unlock the chest\nand seek the awesome\n"
                        "treasure inside!\n\n\n\n"
                        "WASD   Move\nSpace  Jump\nESC    Restart";
    
    s->flash_timer = 0;
    s->entered = 0;
}

static void gameplay_startscreen_fini(const void *handle)
{
}

static void gameplay_startscreen_step(void *handle,
        const struct input_state *input, struct render_list *list)
{
    struct startscreen_state *s = handle;
    int show_text = 0;
    
    if (!s->entered) {
        s->flash_timer = (s->flash_timer + 1)%45;
        
        if (input->any_key) {
            /* any key has been pressed! */
            struct gameplay_state st;
            gameplay_level(&st);
            game_start_state_transition(s->game, &st);
            s->flash_timer = 0;
            
            s->entered = 1;
            game_play_sound(&s->game->start);
        }
        
        show_text = s->flash_timer < 30;
    } else {
        s->flash_timer = (s->flash_timer + 1)%4;
        show_text = s->flash_timer < 2;
    }
    
    if (show_text) {
        render_list_add_rxy(list, ENTITY_text, &s->menu_text, 0, 36, 200);
    }
    render_list_add_rxy(list, ENTITY_text, &s->info_text, 0, 36, 50);
    render_list_set_bg(list, 0, 0, 0);
}

void gameplay_startscreen(struct gameplay_state *s)
{
    GAMEPLAY_STATE_INIT(s, startscreen);
}

