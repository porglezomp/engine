#include "game.h"

#include <stdio.h>
#include <iso646.h>

#include <SDL.h>
#include <SDL_opengl.h>


struct game_state {
    bool quit;
};

static void
game_init(struct game_state *state)
{
    state->quit = false;
}

static void
game_reload(struct game_state *state)
{
     (void) state;
}

static void
game_input(struct game_state *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            state->quit = true;
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            }
        }
    }
}

static bool
game_step(struct game_state *state)
{
    return not state->quit;
}

static void
game_render(struct game_state *state, SDL_Window *window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    (void) state;
    (void) window;
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
    printf("\n\nShutting Down!\n\n");
}


const struct game_api GAME_API = {
    .game_state_size = sizeof(struct game_state),
    .init = game_init,
    .finalize = game_finalize,
    .reload = game_reload,
    .unload = game_unload,
    .input = game_input,
    .step = game_step,
    .render = game_render,
};
