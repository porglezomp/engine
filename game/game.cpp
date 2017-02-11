#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <iso646.h>
#include <stdint.h>
#include <math.h>

#include <SDL.h>

#include "lib/gl.h"
#include "lib/matrix.h"
#include "lib/vector.h"
#include "lib/resources/resource.h"
#include "lib/resources/shader.h"
#include "lib/resources/model.h"
#include "lib/resources/texture.h"

const static GLfloat S = 1, f = 1000, n = 0.1;
const static Mat4 perspective = {{
    S, 0, 0, 0,
    0, S, 0, 0,
    0, 0, -f/(f-n), -1,
    0, 0, -(f*n)/(f-n), 0,
}};

struct Key {
    bool left, right, up, down;
    bool shift, control;
    bool turbo;
};

struct Game_State {
    bool quit;
    bool paused;
    float pitch;
    float yaw;
    Vec4 pos;
    SDL_GameController *controller;
    float left_x, left_y, right_x, right_y;
    float up_movement;
    Resource_Set *shader_set;
    Resource_Set *model_set;
    Resource_Set *texture_set;
    Key key;
};

static void
pause(Game_State *state)
{
    state->paused = true;
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

static void
unpause(Game_State *state)
{
    state->paused = false;
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

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
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

static void
game_input(Game_State *state)
{
    if (state->controller) {
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

        state->up_movement -= SDL_GameControllerGetAxis(state->controller,
                                                        SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        state->up_movement += SDL_GameControllerGetAxis(state->controller,
                                                        SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        state->up_movement /= INT16_MAX;
    } else {
        state->left_x = 0;
        state->left_y = 0;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT)
            state->quit = true;

        switch (event.type) {
        case SDL_MOUSEMOTION:
            if (not state->controller) {
                state->left_x = event.motion.xrel;
                state->left_y = event.motion.yrel;
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_A:
                state->key.turbo = true;
                break;
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
            switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_A:
                state->key.turbo = false;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                if (state->paused) {
                    unpause(state);
                } else {
                    pause(state);
                }
                break;
            case SDLK_UP:
            case SDLK_w:
                state->key.up = false;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                state->key.down = false;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                state->key.left = false;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                state->key.right = false;
                break;
            case SDLK_LSHIFT:
                state->key.shift = false;
                break;
            case SDLK_LCTRL:
                state->key.control = false;
                break;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                state->key.up = true;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                state->key.down = true;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                state->key.left = true;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                state->key.right = true;
                break;
            case SDLK_LSHIFT:
                state->key.shift = true;
                break;
            case SDLK_LCTRL:
                state->key.control = true;
                break;
            }
            break;
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                unpause(state);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                pause(state);
                break;
            }
            break;
        }
    }

    if (not state->controller) {
        state->right_x = state->right_y = 0;
        state->up_movement = 0;
        if (state->key.up)
            state->right_y += 1;
        if (state->key.down)
            state->right_y -= 1;
        if (state->key.right)
            state->right_x += 1;
        if (state->key.left)
            state->right_x -= 1;
        if (state->key.shift)
            state->up_movement += 1;
        if (state->key.control)
            state->up_movement -= 1;
    }
}

static bool
game_step(Game_State *state)
{
    if (state->paused) {
        return not state->quit;
    }

    state->yaw -= state->left_x / 30;
    state->pitch += state->left_y / 30;
    if (state->pitch >= M_PI_2) {
        state->pitch = M_PI_2;
    } else if (state->pitch <= -M_PI_2) {
        state->pitch = -M_PI_2;
    }
    Mat4 rotation = mat4_rotation_y(state->yaw);

    float s = state->key.turbo ? 0.2 : 0.1;
    Vec4 vec = {-state->right_x * s, -state->up_movement * s, state->right_y * s, 0};
    vec = mat4_lmul_vec(&rotation, &vec);
    vec4_addi(&state->pos, &vec);
    return not state->quit;
}

static void
game_render(Game_State *state, SDL_Window *window)
{
    if (state->paused) {
        SDL_Delay(50);
        return;
    }

    glClearColor(0.016, 0.039, 0.247, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 yaw_rotation = mat4_rotation_y(-state->yaw);
    Mat4 pitch_rotation = mat4_rotation_x(state->pitch);
    Mat4 translation = mat4_translation(state->pos.x, state->pos.y, state->pos.z);

    Mat4 base_model_view = pitch_rotation;
    mat4_muli(&base_model_view, &yaw_rotation);
    mat4_muli(&base_model_view, &translation);

    Model_Resource *model = (Model_Resource*) state->model_set->set[2].resource;
    Texture_Resource *texture1 = (Texture_Resource*)state->texture_set->set[0].resource;
    Texture_Resource *texture2 = (Texture_Resource*)state->texture_set->set[1].resource;
    Shader_Resource *shader = model->shader;

    bind_model(model);
    glUniformMatrix4fv(shader->uniforms[1], 1, GL_FALSE, perspective.entries);
    glUniformMatrix4fv(shader->uniforms[0], 1, GL_FALSE, base_model_view.entries);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1->texture_handle);
    glUniform1i(shader->uniforms[2], 0);
    glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0);

    model = (Model_Resource*) state->model_set->set[3].resource;
    bind_model(model);
    glBindTexture(GL_TEXTURE_2D, texture2->texture_handle);
    glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}

static void
game_unload(Game_State *state)
{
    SDL_GameControllerClose(state->controller);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    state->controller = NULL;
}

static void
game_finalize(Game_State *state)
{
    (void) state;
}

static void
game_send_set(Game_State *state, Set_Type type, Resource_Set *queue)
{
    switch (type) {
    case Set_Type_Shader:
        state->shader_set = queue;
        break;
    case Set_Type_Model:
        state->model_set = queue;
        break;
    case Set_Type_Texture:
        state->texture_set = queue;
        break;
    }
}

extern "C" {

const Game_Api GAME_API = {
    sizeof(Game_State),
    game_init,
    game_finalize,
    game_reload,
    game_unload,
    game_input,
    game_step,
    game_render,
    game_send_set,
};

}
