#include "game.h"

#include <assert.h>
#include <stdio.h>
#include <iso646.h>
#include <stdint.h>

#include <SDL.h>

#include "lib/gl.h"
#include "lib/matrix.h"
#include "lib/vector.h"
#include "lib/resources/resource.h"
#include "lib/resources/shader.h"
#include "lib/resources/model.h"

extern "C" {

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
};

struct Game_State {
    bool quit;
    float pitch;
    float yaw;
    Vec4 pos;
    SDL_GameController *controller;
    float left_x, left_y, right_x, right_y;
    float up_movement;
    Resource_Set *shader_set;
    Resource_Set *model_set;
    Key key;
};

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
    } else {
        state->left_x = 0;
        state->left_y = 0;
    }

    state->up_movement = 0;

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
                state->key.shift = false;
                break;
            case SDLK_LCTRL:
                state->key.control = false;
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
}

static bool
game_step(Game_State *state)
{
    state->yaw -= state->left_x / 30;
    state->pitch += state->left_y / 30;
    Mat4 yaw_rotation = mat4_rotation_y(state->yaw);
    Mat4 rotation = mat4_rotation_x(-state->pitch);
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

    Mat4 yaw_rotation = mat4_rotation_y(-state->yaw);
    Mat4 pitch_rotation = mat4_rotation_x(state->pitch);
    Mat4 translation = mat4_translation(state->pos.x, state->pos.y, state->pos.z);

    Mat4 base_model_view = pitch_rotation;
    mat4_muli(&base_model_view, &yaw_rotation);
    mat4_muli(&base_model_view, &translation);

    Model_Resource *model = (Model_Resource*) state->model_set->set[0].resource;
    Shader_Resource *shader = model->shader;
    bind_model(model);
    glUniformMatrix4fv(shader->uniforms[1], 1, GL_FALSE, perspective.entries);

    const int grid_size = 30;
    for (int x = -grid_size; x <= grid_size; ++x) {
        for (int y = -grid_size; y <= grid_size; ++y) {
            Mat4 local = mat4_translation(x * 10, 0, y * 10);
            Mat4 model_view = mat4_mul(&base_model_view, &local);
            glUniformMatrix4fv(shader->uniforms[0], 1, GL_FALSE, model_view.entries);
            glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0);
        }
    }

    SDL_GL_SwapWindow(window);
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
    }
}


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
