#include "game.h"

#include <stdio.h>
#include <iso646.h>

#include <SDL.h>
#include <SDL_opengl.h>


typedef struct Game_State {
    bool quit;
} Game_State;

static void
game_init(Game_State *state)
{
    state->quit = false;
}

static void
game_reload(Game_State *state)
{
     (void) state;
}

static void
game_input(Game_State *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT)
            state->quit = true;

        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            }
        }
    }
}

static bool
game_step(Game_State *state)
{
    return not state->quit;
}

static void
game_render(Game_State *state, SDL_Window *window)
{
    glClearColor(0.016, 0.039, 0.247, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    (void) state;
    (void) window;
}

static void
game_unload(Game_State *state)
{
    (void) state;
}

static void
game_finalize(Game_State *state)
{
    (void) state;
}


const Game_Api GAME_API = {
    .game_state_size = sizeof(Game_State),
    .init = game_init,
    .finalize = game_finalize,
    .reload = game_reload,
    .unload = game_unload,
    .input = game_input,
    .step = game_step,
    .render = game_render,
};
