#include "game.h"

#include <stdio.h>
#include <iso646.h>
#include <stdint.h>

#include <SDL.h>

#include "lib/gl.h"
#include "lib/matrix.h"
#include "lib/vector.h"


const static GLfloat S = 1, f = 1000, n = 0.1;
const static Mat4 perspective = (Mat4) {{
    S, 0, 0, 0,
    0, S, 0, 0,
    0, 0, -f/(f-n), -1,
    0, 0, -(f*n)/(f-n), 0,
}};

typedef struct Game_State {
    bool quit;
    float pitch;
    float yaw;
    Vec4 pos;
    SDL_GameController *controller;
    float left_x, left_y, right_x, right_y;
    float up_movement;
} Game_State;

static void
game_init(Game_State *state)
{
    state->quit = false;
    state->pos.z = 5;
    state->pos.y = -7;
    state->pos.z = 0;
}

static void
game_reload(Game_State *state)
{
    state->controller = SDL_GameControllerOpen(0);
    if (state->controller == NULL) {
        printf("Error opening joystick: %s\n", SDL_GetError());
    }
}

static void
game_input(Game_State *state)
{
    state->left_x = SDL_GameControllerGetAxis(state->controller,
                                              SDL_CONTROLLER_AXIS_LEFTX);
    state->left_y = -SDL_GameControllerGetAxis(state->controller,
                                               SDL_CONTROLLER_AXIS_LEFTY);
    state->left_x /= INT16_MAX;
    state->left_y /= INT16_MAX;

    state->right_y = -SDL_GameControllerGetAxis(state->controller,
                                                SDL_CONTROLLER_AXIS_RIGHTY);
    state->right_x = SDL_GameControllerGetAxis(state->controller,
                                               SDL_CONTROLLER_AXIS_RIGHTX);
    state->right_y /= INT16_MAX;
    state->right_x /= INT16_MAX;

    state->up_movement = 0;

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT)
            state->quit = true;

        switch (event.type) {
        case SDL_CONTROLLERBUTTONUP:
            switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                state->up_movement = 1;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                state->up_movement = -1;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                break;
            case SDLK_TAB:
                break;
            }
            break;
        }
    }
}

static bool
game_step(Game_State *state)
{
    state->yaw -= state->left_x / 30;
    state->pitch += state->left_y / 30;
    Mat4 yaw_rotation = mat4_rotation_y(-state->yaw);
    Mat4 rotation = mat4_rotation_x(state->pitch);
    mat4_muli(&rotation, &yaw_rotation);

    Vec4 vec = {-state->right_x / 10, -state->up_movement, state->right_y / 10, 0};
    vec = mat4_lmul_vec(&rotation, &vec);
    vec4_addi(&state->pos, &vec);
    return not state->quit;
}

static void
game_render(Game_State *state, SDL_Window *window)
{
    glClearColor(0.016, 0.039, 0.247, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 yaw_rotation = mat4_rotation_y(state->yaw);
    Mat4 pitch_rotation = mat4_rotation_x(state->pitch);
    Mat4 translation = mat4_translation(state->pos.x, state->pos.y, state->pos.z);

    Mat4 model_view_matrix = perspective;
    mat4_muli(&model_view_matrix, &pitch_rotation);
    mat4_muli(&model_view_matrix, &yaw_rotation);
    mat4_muli(&model_view_matrix, &translation);

    glUniformMatrix4fv(0, 1, GL_TRUE, model_view_matrix.entries);
    glDrawElements(GL_TRIANGLES, 5979, GL_UNSIGNED_INT, 0);

    (void) window;
}

static void game_unload(Game_State *state)
{
    SDL_GameControllerClose(state->controller);
    state->controller = NULL;
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
