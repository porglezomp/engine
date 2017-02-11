#ifndef GAMELIB_HEADER_INCLUDED
#define GAMELIB_HEADER_INCLUDED

#include <stddef.h>
#include <stdbool.h>

extern "C" {

struct SDL_Window;
struct Game_State;
struct Resource_Set;

enum Set_Type {
    Set_Type_Shader,
    Set_Type_Model,
    Set_Type_Texture,
};

struct Game_Api {
    size_t game_state_size;
    void (*init)(Game_State *state);
    void (*finalize)(Game_State *state);
    void (*reload)(Game_State *state);
    void (*unload)(Game_State *state);
    void (*input)(Game_State *state);
    bool (*step)(Game_State *state);
    void (*render)(Game_State *state, SDL_Window *window);
    void (*send_set)(Game_State *state, Set_Type type, Resource_Set *set);
};

extern const Game_Api GAME_API;

}


#endif // GAMELIB_HEADER_INCLUDED
