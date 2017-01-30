#include "game.h"

struct game_state {
    int placeholder;
};

static void
game_init(struct game_state *state)
{
    (void) state;
}

static void
game_reload(struct game_state *state)
{
    (void) state;
}

static bool
game_step(struct game_state *state)
{
    (void) state;
    return false;
}

static void
game_unload(struct game_state *state)
{
    (void) state;
}

static void
game_finalize(struct game_state *state)
{
    (void) state;
}

const struct game_api GAME_API = {
    .game_state_size = sizeof(struct game_state),
    .init = game_init,
    .reload = game_reload,
    .step = game_step,
    .unload = game_unload,
    .finalize = game_finalize,    
};
