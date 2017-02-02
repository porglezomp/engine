#ifndef GAMELIB_HEADER_INCLUDED
#define GAMELIB_HEADER_INCLUDED

#include <stddef.h>
#include <stdbool.h>

#include <SDL.h>

struct game_state;

struct game_api {
    size_t game_state_size;
    void (*init)(struct game_state *state);
    void (*finalize)(struct game_state *state);
    void (*reload)(struct game_state *state);
    void (*unload)(struct game_state *state);
    void (*input)(struct game_state *state);
    bool (*step)(struct game_state *state);
    void (*render)(struct game_state *state, SDL_Window *window);
};

extern const struct game_api GAME_API;

#endif // GAMELIB_HEADER_INCLUDED
