#ifndef GAMELIB_HEADER_INCLUDED
#define GAMELIB_HEADER_INCLUDED

#include <stddef.h>
#include <stdbool.h>

struct game_state;

struct game_api {
    size_t game_state_size;
    void (*init)(struct game_state *state);
    void (*finalize)(struct game_state *state);
    void (*reload)(struct game_state *state);
    void (*unload)(struct game_state *state);
    bool (*step)(struct game_state *state);
};

extern const struct game_api GAME_API;

#endif // GAMELIB_HEADER_INCLUDED
