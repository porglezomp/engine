#include <stdio.h>
#include <unistd.h>
#include <iso646.h>
#include <dlfcn.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <dirent.h>

#include <SDL.h>

#include "lib/gl.h"

#include "game/game.h"

#include "load_shader.h"
#include "load_model.h"
#include "hotload.h"
#include "entity.h"

#include "lib/matrix.h"
#include "lib/resources/model.h"
#include "lib/resources/shader.h"

#define WIDTH 640
#define HEIGHT 480

static const char *GAME_LIBRARY = "./target/libgame.so";
static bool should_reload = false;
static bool game_interrupted = false;

struct Game {
    void *handle;
    Game_Api api;
    Game_State *state;
};

static SDL_Window *window;
static SDL_GLContext context;

static void handle_load_signal(int);
static void handle_quit_signal(int);
static void install_signals(void);
static void game_load(Game*);
static void game_unload(Game*);
static bool init_sdl(void);


// Main

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

Resource_Set shader_set;
Resource_Set model_set;

void
reload_shaders(const char *filename)
{
    for (size_t i = 0; i < shader_set.count; ++i) {
        Shader_Resource *shader = (Shader_Resource*) shader_set.set[i].resource;
        if (same_suffix(filename, shader->vert_fname) or
            same_suffix(filename, shader->frag_fname)) {
            Resource_Error resource_error = {0};
            if (shader_load(shader, &resource_error)) {
                printf("Error loading shader: %s\n", resource_error.message);
                free_resource_error(&resource_error);
            }
        }
    }
}

void
reload_models(const char *filename)
{
    for (size_t i = 0; i < model_set.count; ++i) {
        Model_Resource *model = (Model_Resource*) model_set.set[i].resource;
        if (same_suffix(filename, "assets/tree.model")) {
            Resource_Error resource_error = {0};
            if (model_load(model, &resource_error)) {
                printf("Error loading model: %s\n", resource_error.message);
                free_resource_error(&resource_error);
            }
        }
    }
}

int
main()
{
    static Game game;
    install_signals();
    bool running = init_sdl();

    Resource_Error resource_error = {0};
    Shader_Resource *shader;
    if (shader_set_add(&shader_set, "assets/shader.vert", "assets/shader.frag",
                       &shader, &resource_error)) {
        printf("Error loading shader: %s\n", resource_error.message);
        free_resource_error(&resource_error);
        SDL_DestroyWindow(window);
        return 1;
    }
    const char *names[] = {"modelview", "perspective"};
    shader_bind_uniforms(shader, 2, names);

    Model_Resource *model;
    if (model_set_add(&model_set, "assets/tree.model", &model, &resource_error)) {
        printf("Error loading model: %s\n", resource_error.message);
        resource_set_free(&shader_set);
        free_resource_error(&resource_error);
        SDL_DestroyWindow(window);
        return 1;
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("assets/world")) != NULL) {
        char fname[1024];
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') continue;
            snprintf(fname, 1024, "assets/world/%s", ent->d_name);
            FILE *f = fopen(fname, "r");
            Entity entity;
            entity_load_file(&entity, f);
            fclose(f);
        }
        closedir(dir);
    } else {
        printf("Error loading world\n");
        printf("Error loading model: %s\n", resource_error.message);
        resource_set_free(&shader_set);
        free_resource_error(&resource_error);
        SDL_DestroyWindow(window);
        return 1;
    }

    model->shader = shader;

    register_hotload_callback(reload_shaders);
    register_hotload_callback(reload_models);

    game_load(&game);
    if (not game.handle)
        return 1;

    game.api.send_set(game.state, Set_Type_Model, &model_set);
    game.api.send_set(game.state, Set_Type_Shader, &shader_set);
    assert(glGetError() == GL_NO_ERROR);

    while (running and not game_interrupted) {
        if (should_reload)
            game_load(&game);

        if (game.handle) {
            game.api.input(game.state);
            running = game.api.step(game.state);
            game.api.render(game.state, window);
        }

        // @Todo: Better framerate handling
        // SDL_Delay(1000/60);
        run_hotload_callbacks();
    }

    resource_set_free(&shader_set);
    resource_set_free(&model_set);
    game_unload(&game);
    SDL_DestroyWindow(window);
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
        const Game_Api *api = (Game_Api*) dlsym(game->handle, "GAME_API");
        if (api != NULL) {
            game->api = *api;
            if (game->state == NULL) {
                game->state = (Game_State*) calloc(1, game->api.game_state_size);
                game->api.init(game->state);
            } else {
                game->state = (Game_State*) realloc(game->state, game->api.game_state_size);
            }
            game->api.reload(game->state);
        } else {
            dlclose(game->handle);
            game->handle = NULL;
        }
    } else {
        game->handle = NULL;
        printf("Failed to open %s\n", GAME_LIBRARY);
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL could not be initialized: %s\n", SDL_GetError());
        return false;
    }

    atexit(SDL_Quit);

    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

    window = SDL_CreateWindow("The Garden",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WIDTH, HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("Window could not be created: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_GL_SetSwapInterval(-1)) {
        SDL_GL_SetSwapInterval(1);
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    return true;
}
