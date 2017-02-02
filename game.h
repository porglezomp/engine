#ifndef GAMELIB_HEADER_INCLUDED
#define GAMELIB_HEADER_INCLUDED

#include <stddef.h>
#include <stdbool.h>

typedef struct SDL_Window SDL_Window;
typedef struct Game_State Game_State;

typedef struct Game_Api {
    size_t game_state_size;
    void (*init)(Game_State *state);
    void (*finalize)(Game_State *state);
    void (*reload)(Game_State *state);
    void (*unload)(Game_State *state);
    void (*input)(Game_State *state);
    bool (*step)(Game_State *state);
    void (*render)(Game_State *state, SDL_Window *window);
} Game_Api;

extern const Game_Api GAME_API;

#endif // GAMELIB_HEADER_INCLUDED
