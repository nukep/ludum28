#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "state.h"
#include "entities.h"
#include "tiles.h"
#include "object.h"
#include "level.h"
#include "collision.h"

struct particle {
    int active;
    struct entity_blob blob;
    struct object obj;
    int tick;
    int lifespan;
};

struct sand {
    int active;
    int x, y;
    int tick;
};

static const int MAX_PARTICLES = 16;
static const int MAX_SAND = 16;

struct level_state {
    const struct game    *game;
    struct level          level;
    
    struct entity_blob    player;
    struct entity_sprite  player_face;
    struct entity_bgtiles bgtiles;
    struct entity_sprite  key_sprite;
    struct entity_text    message;
    
    int particle_index;
    struct particle       *particles;
    
    int sand_index;
    struct sand           *sand;
    
    float player_rot;
    int player_dir_right;
    
    int key_rot;
    
    struct object play;
    struct object key;
    int player_has_key;
    int player_dead;
    int player_dead_tick;
    int player_in_liquid;
    int player_climbing;
    int player_found_chest;
    
    int cam_x, cam_y;
};

static void add_particle(struct level_state *l,
                         float x, float y, float vel_x, float vel_y,
                         float r, float g, float b,
                         int lifespan)
{
    struct particle *p = l->particles + l->particle_index;
    l->particle_index = (l->particle_index+1) % MAX_PARTICLES;
    
    p->active = 1;
    p->blob.speed = 0;
    p->blob.r = r;
    p->blob.g = g;
    p->blob.b = b;
    object_init(&p->obj, x, y, 2, 2, 1);
    p->obj.vel_x = vel_x;
    p->obj.vel_y = vel_y;
    p->tick = 0;
    p->lifespan = lifespan;
}

static void set_camera(struct level_state *l, struct render_list *list)
{
    const int center_x = 128;
    const int center_y = 160;
    const int right_x  = (l->level.width-16)*16;
    const int bottom_y  = (l->level.height-16)*16;
    int cam_x = l->play.pos_x - center_x;
    int cam_y = l->play.pos_y - center_y;
    
    if (cam_x < 0) {
        cam_x = 0;
    }
    if (cam_y < 0) {
        cam_y = 0;
    }
    
    if (cam_x > right_x) {
        cam_x = right_x;
    }
    if (cam_y > bottom_y) {
        cam_y = bottom_y;
    }
    
    l->cam_x = cam_x;
    l->cam_y = cam_y;
    
    render_list_set_camera(list, cam_x, cam_y);
}

static int get_tile(const void *handle, int x, int y)
{
    const struct level_state *l = handle;
    
    if (x < 0 || y < 0 || x >= l->level.width || y >= l->level.height) {
        return 0;
    } else {
        return l->level.map_tiles[l->level.width*y + x];
    }
}

static int get_tile_solid(const void *handle, int x, int y)
{
    const struct level_state *l = handle;
    
    if (x < 0 || x >= l->level.width) {
        /* anything out of bounds is "solid" */
        return 2;
    } else {
        int tile = get_tile(handle, x, y);
        switch (tile) {
        case 0:
        case T_GRASS_TOP:
        case T_LADDER:
        case T_COIN:
        case T_CHEST:
        case T_CHEST_OPEN:
            /* there might be a boulder */
            for (int i = 0; i < l->level.boulder_count; i++) {
                const struct level_boulder *b = l->level.boulder+i;
                
                if (b->state == BS_DOWN) {
                    if (x >= b->x/16 && x < (b->x+b->w)/16 &&
                        y >= b->y/16 && y < (b->y+b->h)/16)
                    {
                        return 1;
                    }
                }
            }
            return 0;
        default:
            return 2;
        }
    }
}

static void gameplay_level_init(void *handle, const struct game *game)
{
    struct level_state *l = handle;
    
    l->game = game;
    level_generate(&l->level);
    
    l->bgtiles.tileset = &game->tileset;
    l->bgtiles.handle = l;
    l->bgtiles.w = l->level.width;
    l->bgtiles.h = l->level.height;
    l->bgtiles.get_tile = get_tile;
    
    l->key_sprite.tileset = &game->tileset;
    l->key_sprite.tile = T_KEY;
    l->key_rot = 0;
    
    l->particle_index = 0;
    l->particles = malloc(MAX_PARTICLES * sizeof *l->particles);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        l->particles[i].active = 0;
    }
    
    l->sand_index = 0;
    l->sand = malloc(MAX_SAND * sizeof *l->sand);
    for (int i = 0; i < MAX_SAND; i++) {
        l->sand[i].active = 0;
    }
    
    l->message.font = &game->main_font;
    l->message.text = "";
    
    l->player.speed = 0;
    l->player.r     = 1;
    l->player.g     = 1;
    l->player.b     = 1;
    
    l->player_face.tileset = &game->tileset;
    l->player_face.tile = T_PLAYER_FACE;
    
    l->player_rot = 0;
    l->player_dir_right = 1;
    
    object_init(&l->key, 80, 72, 16, 8, 0.8);
    l->key.vel_x = 200;
    l->key.vel_y = -50;
    l->key.perp_friction = 0.5;
    l->key.permeability = 2;
    
    object_init(&l->play, l->level.spawn_x, l->level.spawn_y, 8, 24, 0.2);
    l->play.circle = 0;
    l->player_has_key = 0;
    l->player_dead = 0;
    l->player_dead_tick = 0;
    l->player_climbing = 0;
    l->player_in_liquid = 0;
    l->player_found_chest = 0;
    
    srand((unsigned)time(NULL));
}

static void gameplay_level_fini(const void *handle)
{
    const struct level_state *l = handle;
    free(l->particles);
    level_fini(&l->level);
}

static void player_dies(struct level_state *l);

static void draw_player(const struct level_state *l, struct render_list *list)
{
    float dir_mul = l->player_dir_right?1:-1;
    float x = l->play.pos_x+12-8;
    float y = l->play.pos_y+12;
    float blob_angle = atan2(l->play.vel_y, l->play.vel_x)*180/3.1415;
    
    render_list_add_osrxy(list, ENTITY_blob, &l->player,
                          0, 0, dir_mul*12, 12, blob_angle,
                          x, y);
    render_list_add_osrxy(list, ENTITY_sprite, &l->player_face,
                          0, 8.5, dir_mul*1, 1, l->player_rot,
                          x, y);
}

static void draw_key(const struct level_state *l, struct render_list *list)
{
    render_list_add_osrxy(list, ENTITY_sprite, &l->key_sprite,
                          8.5, 8.5, 1, 1, l->key_rot, l->key.pos_x+8, l->key.pos_y);
}

static void draw_liquid(const struct level_state *l, struct render_list *list)
{
    for (int i = 0; i < l->level.water_count; i++) {
        render_list_add_rxy(list, ENTITY_liquid, &l->level.water[i].l,
                            0, l->level.water[i].x, l->level.water[i].y);
    }
    for (int i = 0; i < l->level.lava_count; i++) {
        render_list_add_rxy(list, ENTITY_liquid, &l->level.lava[i].l,
                            0, l->level.lava[i].x, l->level.lava[i].y);
    }
}

static void draw_particles(const struct level_state *l, struct render_list *list)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        const struct particle *p = l->particles + i;
        if (p->active) {
            float angle = atan2(p->obj.vel_y, p->obj.vel_x)*180/3.1415;
            
            render_list_add_osrxy(list, ENTITY_blob, &p->blob,
                                  0, 0, 2, 2, angle, p->obj.pos_x, p->obj.pos_y);
        }
    }
}

static void draw_boulders(const struct level_state *l, struct render_list *list)
{
    for (int i = 0; i < l->level.boulder_count; i++) {
        int x = l->level.boulder[i].x;
        x += 1*(((l->level.boulder[i].tick/2)%2)==0?1:-1);
        render_list_add_rxy(list, ENTITY_shape, &l->level.boulder[i].shape,
                            0, x, l->level.boulder[i].y);
    }
}

static void update_liquid(struct level_state *l)
{
    for (int i = 0; i < l->level.water_count; i++) {
        l->level.water[i].l.phase++;
    }
    for (int i = 0; i < l->level.lava_count; i++) {
        l->level.lava[i].l.phase++;
    }
}

static void update_particles(struct level_state *l)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        struct particle *p = l->particles + i;
        if (p->active) {
            object_step(&p->obj, 0, 0);
            p->blob.speed = hypot(p->obj.vel_x, p->obj.vel_y)/20;
            p->tick++;
            if (p->tick >= p->lifespan) {
                p->active = 0;
            }
        }
    }
}

static void update_boulders(struct level_state *l)
{
    const int RANGE = 2;
    struct collision_box player;
    object_get_collision_box(&l->play, &player);
    
    for (int i = 0; i < l->level.boulder_count; i++) {
        struct level_boulder *b = l->level.boulder+i;
        
        if (b->state == BS_UNACTIVATED) {
            const struct collision_box range =
                {.x = b->x - 16*RANGE, .y = b->y,
                 .w = b->w + 16*RANGE*2, .h = b->range_h};
            
            if (collision_test_box_box(&player, &range)) {
                b->state = BS_SHAKE;
                game_play_sound(&l->game->boulder_budge);
            }
        } else if (b->state == BS_SHAKE) {
            b->tick++;
            if (b->tick >= 20) {
                b->tick = 0;
                b->state = BS_FALLING;
            }
        } else if (b->state == BS_FALLING) {
            const int dest_y = b->original_y + b->range_h - b->h;
            b->y += b->vel/2;
            
            const struct collision_box box =
                {.x = b->x - 16*RANGE, .y = b->y,
                 .w = b->w + 16*RANGE*2, .h = b->h};
            
            if (collision_test_box_box(&player, &box)) {
                player_dies(l);
            }
            
            if (b->vel < 20) {
                b->vel++;
            }
            if (b->y >= dest_y) {
                b->y = dest_y;
                b->state = BS_DOWN;
                game_play_sound(&l->game->boulder_down);
            }
        }
    }
}

static void check_player_in_liquid(struct level_state *l)
{
    struct collision_box player;
    object_get_collision_box(&l->play, &player);
    
    int old = l->player_in_liquid;
    int lava = 0;
    l->player_in_liquid = 0;
    
    for (int i = 0; i < l->level.water_count; i++) {
        struct collision_box box =
            {.x = l->level.water[i].x, .y = l->level.water[i].y,
             .w = l->level.water[i].l.w, .h = l->level.water[i].l.h};
        
        if (collision_test_box_box(&player, &box)) {
            l->player_in_liquid = 1;
        }
    }
    for (int i = 0; i < l->level.lava_count; i++) {
        struct collision_box box =
            {.x = l->level.lava[i].x, .y = l->level.lava[i].y,
             .w = l->level.lava[i].l.w, .h = l->level.lava[i].l.h};
        
        if (collision_test_box_box(&player, &box)) {
            lava = 1;
        }
    }
    
    int show_particles = 0;
    
    if (lava) {
        player_dies(l);
    } else if (!old && l->player_in_liquid) {
        game_play_sound(&l->game->splash);
        show_particles = 1;
    }
    
    if (show_particles) {
        for (int i = -3; i <= +3; i++) {
            add_particle(l, l->play.pos_x+4+i*4, l->play.pos_y+24, i*20, -150, 1, 1, 1, 45);
        }
    }
}

static void player_gets_key(struct level_state *l)
{
    game_play_sound(&l->game->key_pickup);
    l->player_has_key = 1;
}

static void player_loses_key(struct level_state *l)
{
    if (l->player_has_key) {
        l->player_has_key = 0;
        l->key.pos_x = l->play.pos_x;
        l->key.pos_y = l->play.pos_y;
        l->key.vel_y = -70;
        l->key.vel_x = 70 * (rand()%2>0?1:-1);
    }
}

static void player_dies(struct level_state *l)
{
    game_play_sound(&l->game->death);
    l->player_dead = 1;
    l->player_dead_tick = 60;
    player_loses_key(l);
}

static void player_jumps(struct level_state *l)
{
    game_play_sound(&l->game->jump);
    l->play.vel_y = -200;
}

static void key_collides(struct level_state *l)
{
    game_play_sound(&l->game->key_ding);
}

static int player_is_on_ground(const struct level_state *l)
{
    struct collision_box player_box;
    object_get_collision_box(&l->play, &player_box);
    
    int y = floorf((l->play.pos_y+l->play.bound_h+1)/16);
    
    return get_tile_solid(l, floorf(l->play.pos_x/16), y) ||
           get_tile_solid(l, floorf((l->play.pos_x+l->play.bound_w)/16), y);
}

static int is_player_on_ladder(const struct level_state *l)
{
    int x = l->play.pos_x/16;
    int y = (l->play.pos_y+16)/16;
    return get_tile(l, x,y) == T_LADDER || get_tile(l, x,y+1) == T_LADDER;
}

static int is_player_near_tile(const struct level_state *l, int tile,
                               int *ox, int *oy, int x_off, int y_off)
{
    int x = l->play.pos_x+x_off;
    int y = l->play.pos_y+y_off;
    
    int has_tile = 0;
    
    if (get_tile(l, x/16,y/16) == tile) {
        has_tile = 1;
        *ox = x/16;
        *oy = y/16;
    } else if (get_tile(l, (x+8)/16,y/16) == tile) {
        has_tile = 1;
        *ox = (x+8)/16;
        *oy = y/16;
    } else if (get_tile(l, x/16,(y+16)/16) == tile) {
        has_tile = 1;
        *ox = (x)/16;
        *oy = (y+16)/16;
    } else if (get_tile(l, (x+8)/16,(y+16)/16) == tile) { 
        has_tile = 1;
        *ox = (x+8)/16;
        *oy = (y+16)/16;
    }
    
    return has_tile;
}

static void check_player_coin(struct level_state *l)
{
    int x, y;
    if (is_player_near_tile(l, T_COIN, &x, &y, 0, 0)) {
        level_set_tile(&l->level, x, y, 0);
        game_play_sound(&l->game->coin);
    }
}

static void check_player_chest(struct level_state *l)
{
    int x, y;
    if (is_player_near_tile(l, T_CHEST, &x, &y, 0, 16) ||
        is_player_near_tile(l, T_CHEST, &x, &y, 16, 0))
    {
        if (l->player_has_key) {
            level_set_tile(&l->level, x, y, T_CHEST_OPEN);
            game_play_sound(&l->game->coin);
            l->player_found_chest = 1;
        } else {
            l->message.text = "You need the key!";
        }
    }
}

static void check_player_sand(struct level_state *l)
{
    int x, y;
    if (is_player_near_tile(l, T_SAND_1, &x, &y, 0, 16))
    {
        /* make sand disintegrate */
        struct sand *s = l->sand + l->sand_index;
        l->sand_index = (l->sand_index+1)%MAX_SAND;
        
        s->active = 1;
        s->tick = 0;
        s->x = x;
        s->y = y;
    }
    
    for (int i = 0; i < MAX_SAND; i++) {
        struct sand *s = l->sand + i;
        if (s->active) {
            s->tick++;
            if (s->tick < 8) {
            } else if (s->tick < 16) {
                level_set_tile(&l->level, s->x, s->y, T_SAND_2);
            } else if (s->tick < 24) {
                level_set_tile(&l->level, s->x, s->y, T_SAND_3);
            } else {
                level_set_tile(&l->level, s->x, s->y, 0);
                s->active = 0;
            }
        }
    }
}

static void gameplay_level_step(void *handle,
        const struct input_state *input, struct render_list *list)
{
    struct level_state *l = handle;
    int old_dir_right = l->player_dir_right;
    
    /* draw */
    
    render_list_add_rxy(list, ENTITY_bgtiles, &l->bgtiles, 0, 0, 0);
    
    draw_liquid(l, list);
    l->player.speed = hypot(l->play.vel_x, l->play.vel_y)/100;
    if (!l->player_dead) {
        draw_player(l, list);
    }
    draw_boulders(l, list);
    
    draw_particles(l, list);
    
    if (l->player_has_key) {
    } else {
        draw_key(l, list);
    }
    
    set_camera(l, list);
    
    render_list_add_rxy(list, ENTITY_text, &l->message, 0, l->cam_x, l->cam_y);
    
    render_list_set_bg(list, 0, 0, 0.2);
    
    /* update physics */
    
    if (!l->player_dead) {
        if (!l->player_climbing) {
            l->play.gravity = l->player_in_liquid?0.2:1.0;
            object_step(&l->play, get_tile_solid, l);
            
            if (fabsf(l->play.vel_x) < 1) {
                /* put face back in normal direction */
                l->player_rot *= 0.8;
            } else {
                l->player_rot = fmodf(l->player_rot + l->play.vel_x/10, 360.0);
                if (l->player_rot > 180) {
                    l->player_rot -= 360;
                } else if (l->player_rot < -180) {
                    l->player_rot += 360;
                }
            }
        } else {
            l->play.vel_x = 0;
            l->play.vel_y = 0;
            l->play.pos_y -= 2;
            if (!is_player_on_ladder(l)) {
                l->play.pos_y += 2;
            }
        }
        
        check_player_coin(l);
        
        check_player_chest(l);
    }
    
    if (!l->player_has_key) {
        int lost = l->key.pos_y > l->level.height*16;
        
        if (lost) {
            l->message.text = "The key is lost!\nPress ESC to reset";
        } else {
            int collided = object_step(&l->key, get_tile_solid, l);
            l->key_rot += fabsf(l->key.vel_x)/10;
            
            if (collided && fabsf(l->key.vel_y) > 10) {
                key_collides(l);
            }
        }
    }
    
    update_liquid(l);
    update_particles(l);
    update_boulders(l);
    
    /* check for input/state */
    
    if (l->player_found_chest) {
        if (l->player_found_chest == 1) {
            l->message.text = "You found the treasure! It's...";
        } else if (l->player_found_chest == 30) {
            struct gameplay_state state;
            gameplay_treasure(&state);
            game_start_state_transition(l->game, &state);
        }
        l->player_found_chest++;
    } else {
        if (!l->player_dead) {
            int old_player_in_liquid = l->player_in_liquid;
            if (input->x > 0) {
                l->play.vel_x = 150;
                l->player_dir_right = 1;
            } else if (input->x < 0) {
                l->play.vel_x = -150;
                l->player_dir_right = 0;
            }
            
            if (input->y < 0) {
                /* climbing? */
                l->player_climbing = is_player_on_ladder(l);
            } else {
                l->player_climbing = 0;
            }
            
            if (old_dir_right != l->player_dir_right) {
                l->player_rot += 180;
            }
            
            if (!l->player_has_key) {
                if (objects_collide(&l->play, &l->key)) {
                    player_gets_key(l);
                }
            }
            
            check_player_in_liquid(l);
            check_player_sand(l);
            
            if (input->action_key) {
                if (player_is_on_ground(l) ||
                    (!l->player_in_liquid && old_player_in_liquid))
                {
                    player_jumps(l);
                } else if (l->player_in_liquid) {
                    l->play.pos_y -= 2;
                    l->play.vel_y = 0;
                }
            }
            
            if (l->play.pos_y > l->level.height*16) {
                player_dies(l);
            }
        } else {
            l->player_dead_tick--;
            if (l->player_dead_tick == 0) {
                l->play.pos_x = l->level.spawn_x;
                l->play.pos_y = l->level.spawn_y;
                l->play.vel_x = 0;
                l->play.vel_y = 0;
                l->player_dead = 0;
            }
        }
        
        if (input->back_key && !l->player_dead) {
            struct gameplay_state state;
            gameplay_startscreen(&state);
            game_start_state_transition(l->game, &state);
        }
    }
}

void gameplay_level(struct gameplay_state *s)
{
    GAMEPLAY_STATE_INIT(s, level);
}

