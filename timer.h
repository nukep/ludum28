#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL_timer.h>

struct timer {
    Uint64 last_ticks;
};

static inline void timer_init(struct timer *timer)
{
    timer->last_ticks = SDL_GetTicks();
}

static inline int timer_elapsed_ms(struct timer *timer)
{
    Uint64 cur = SDL_GetTicks();
    Uint64 delta = cur - timer->last_ticks;
    
    return (int)delta;
}

static inline void timer_reset(struct timer *timer)
{
    timer->last_ticks = SDL_GetTicks();
}

#endif

