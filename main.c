#include <stdio.h>
#include <unistd.h>
#include <iso646.h>
#include <dlfcn.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "gl.h"

#include "game.h"
#include "shader.h"
#include "load_shader.h"
#include "hotload.h"

#define WIDTH 640
#define HEIGHT 480

static const char *GAME_LIBRARY = "./libgame.so";
static bool should_reload = true;
static bool game_interrupted = false;

typedef struct Game {
    void *handle;
    Game_Api api;
    Game_State *state;
} Game;

static Game game;
static SDL_Window *window;
static SDL_GLContext *context;
// @Cleanup: put this in a struct
GLuint vao;
GLuint vbo;
GLuint ibo;
GLfloat verts[][3] = {
    {0, 1, 0},
    {1, 0, 0},
    {-1, 0, 0},
    {0, -1, 0},
};
GLuint indices[][3] = {
    {0, 1, 2},
    {2, 1, 3},
};

static void handle_load_signal(int);
static void handle_quit_signal(int);
static void install_signals(void);
static void game_load(Game*);
static void game_unload(Game*);
static bool init_sdl(void);


// Main

Shader_Resource shader = {
    .vert_fname = "assets/shader.vert",
    .frag_fname = "assets/shader.frag",
    .program = 0,
};

bool
same_suffix(const char *a, const char *b)
{
    size_t a_len = strlen(a), b_len = strlen(b);
    if (a_len > b_len) {
        return strcmp(a + (a_len - b_len), b) == 0;
    } else {
        return strcmp(a, b + (b_len - a_len)) == 0;
    }
}

void
callback(const char *filename)
{
    if (same_suffix(filename, "assets/shader.vert") or
        same_suffix(filename, "assets/shader.frag")) {
        Resource_Error resource_error = {0};
        if (shader_load(&shader, &resource_error)) {
            printf("Error loading shader: %s\n", resource_error.message);
            free_resource_error(&resource_error);
        }
    }
}

int
main()
{
    install_signals();
    bool running = init_sdl();

    Resource_Error resource_error = {0};
    if (shader_load(&shader, &resource_error)) {
        printf("Error loading shader: %s\n", resource_error.message);
        free_resource_error(&resource_error);
        return 1;
    }

    register_hotload_callback(callback);

    // @Cleanup
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3*4 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * sizeof(GLuint), indices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    while (running and not game_interrupted) {
        if (should_reload)
            game_load(&game);

        if (game.handle) {
            game.api.input(game.state);
            running = game.api.step(game.state);

            glUseProgram(shader.program);
            game.api.render(game.state, window);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            assert(glGetError() == GL_NO_ERROR);

            SDL_GL_SwapWindow(window);
        }

        // @Todo: Better framerate handling
        SDL_Delay(1000/60);
        run_hotload_callbacks();
    }

    game_unload(&game);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


// Signal Handling

static void
handle_load_signal(int signal)
{
    (void) signal;
    should_reload = true;
}

static void
handle_quit_signal(int signal)
{
    (void) signal;
    game_interrupted = true;
}

static void
install_signals(void)
{
    signal(SIGUSR1, handle_load_signal);
    signal(SIGHUP, handle_quit_signal);
    signal(SIGINT, handle_quit_signal);
    signal(SIGKILL, handle_quit_signal);
}


// Game Library Reloading

static void
game_load(Game *game)
{
    if (game->handle) {
        game->api.unload(game->state);
        dlclose(game->handle);
    }
    void *handle = dlopen(GAME_LIBRARY, RTLD_NOW);
    if (handle) {
        game->handle = handle;
        const Game_Api *api = dlsym(game->handle, "GAME_API");
        if (api != NULL) {
            game->api = *api;
            if (game->state == NULL) {
                game->state = calloc(1, game->api.game_state_size);
                game->api.init(game->state);
            }
            game->api.reload(game->state);
        } else {
            dlclose(game->handle);
            game->handle = NULL;
        }
    } else {
        game->handle = NULL;
    }
    should_reload = false;
}

static void
game_unload(Game *game)
{
    if (game->handle) {
        game->api.finalize(game->state);
        game->state = NULL;
        dlclose(game->handle);
        game->handle = NULL;
    }
}


// Init SDL

static bool
init_sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("The Garden",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIDTH, HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("Window could not be created: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    context = SDL_GL_CreateContext(window);

    if (context == NULL) {
        printf("Could not create OpenGL context: %s\n", SDL_GetError());
        return false;
    }

    glewInit();

    glClearColor(66.0 / 255, 153.0 / 255, 229.0 / 255, 1.0);

    return true;
}
