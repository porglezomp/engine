#include "game.h"

#include <stdio.h>
#include <iso646.h>
#include <stdint.h>

#include <SDL.h>

#include "lib/gl.h"
#include "lib/matrix.h"


const static GLfloat S = 1, f = 20, n = 0.1;
const static Mat4 perspective = (Mat4) {{
    S, 0, 0, 0,
    0, S, 0, 0,
    0, 0, -f/(f-n), -1,
    0, 0, -(f*n)/(f-n), 0,
}};

typedef struct Game_State {
    bool quit;
    float height;
    float distance;
    float pitch;
    float yaw;
    SDL_GameController *controller;
    float xaxis, yaxis, zoom_axis;
} Game_State;

static void
game_init(Game_State *state)
{
    state->quit = false;
}

static void
game_reload(Game_State *state)
{
    state->height = 5;
    state->distance = 7;

    state->controller = SDL_GameControllerOpen(0);
    if (state->controller == NULL) {
        printf("Error opening joystick: %s\n", SDL_GetError());
    }
}

static void
game_input(Game_State *state)
{
    state->xaxis = SDL_GameControllerGetAxis(state->controller,
                                             SDL_CONTROLLER_AXIS_LEFTX);
    state->yaxis = -SDL_GameControllerGetAxis(state->controller,
                                              SDL_CONTROLLER_AXIS_LEFTY);
    state->xaxis /= INT16_MAX;
    state->yaxis /= INT16_MAX;

    state->zoom_axis = -SDL_GameControllerGetAxis(state->controller,
                                                  SDL_CONTROLLER_AXIS_RIGHTY);
    state->zoom_axis /= INT16_MAX;

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT)
            state->quit = true;

        switch (event.type) {
        case SDL_CONTROLLERBUTTONUP:
            switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_A:
                state->height += 1;
                break;
            case SDL_CONTROLLER_BUTTON_Y:
                state->height -= 1;
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
    state->yaw -= state->xaxis / 10;
    state->pitch += state->yaxis / 10;
    state->distance += state->zoom_axis / 10;
    return not state->quit;
}

static void
game_render(Game_State *state, SDL_Window *window)
{
    glClearColor(0.016, 0.039, 0.247, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 yaw_rotation = mat4_rotation_y(state->yaw);
    Mat4 pitch_rotation = mat4_rotation_x(state->pitch);
    Mat4 translation = mat4_translation(0, -state->height, -state->distance);

    Mat4 model_view_matrix = perspective;
    mat4_muli(&model_view_matrix, &translation);
    mat4_muli(&model_view_matrix, &pitch_rotation);
    mat4_muli(&model_view_matrix, &yaw_rotation);

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
