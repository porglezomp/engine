#include "game.h"

#include <stdio.h>
#include <unistd.h>
#include <iso646.h>
#include <dlfcn.h>
#include <signal.h>
#include <time.h>

static const char *GAME_LIBRARY = "./libgame.so";
static bool should_reload = true;
static bool game_interrupted = false;

struct game {
    void *handle;
    struct game_api api;
    struct game_state *state;
};
struct game zero_game;

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
game_load(struct game *game)
{
    (void) game;
    (void) GAME_LIBRARY;
    printf("Load\n");
    should_reload = false;
}

static void
game_unload(struct game *game)
{
    (void) game;
    printf("Unload\n");
}

int
main()
{
    struct game game = zero_game;
    bool running = true;
    signal(SIGUSR1, handle_load_signal);
    signal(SIGHUP, handle_quit_signal);
    signal(SIGINT, handle_quit_signal);
    signal(SIGKILL, handle_quit_signal);

    while (running and not game_interrupted) {
        if (should_reload) {
            game_load(&game);
        }
        if (game.handle) {
            running = game.api.step(game.state);
        }
        // TODO: Better framerate handling
        sleep(1000/60);
    }

    game_unload(&game);
    return 0;
}
