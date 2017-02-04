#include "game.h"

#include <stdio.h>
#include <iso646.h>


#include <SDL.h>

#include "lib/gl.h"
#include "lib/matrix.h"


typedef struct Game_State {
    bool quit;
    Mat4 perspective;
    Mat4 rotation;
    Mat4 translation;
} Game_State;

static void
game_init(Game_State *state)
{
    state->quit = false;
    GLfloat S = 1, f = 20, n = 0.1;
    state->perspective = (Mat4) {{
        S, 0, 0, 0,
        0, S, 0, 0,
        0, 0, -f/(f-n), -1,
        0, 0, -(f*n)/(f-n), 0,
    }};
    state->rotation = Mat4_Identity;
    state->translation = mat4_translation(0, 0, 2);
}

static void
game_reload(Game_State *state)
{
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    state->translation = mat4_translation(0, 0, -3);
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

    Mat4 next_rotation = mat4_rotation_x(0.01);
    mat4_muli(&state->rotation, &next_rotation);

    Mat4 model_view_matrix = state->perspective;
    mat4_muli(&model_view_matrix, &state->translation);
    mat4_muli(&model_view_matrix, &state->rotation);

    glUniformMatrix4fv(0, 1, GL_TRUE, model_view_matrix.entries);
    glDrawElements(GL_TRIANGLES, 3 * 2 * 6, GL_UNSIGNED_INT, 0);

    (void) window;
}

static void game_unload(Game_State *state)
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
