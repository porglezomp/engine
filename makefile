OS=$(shell uname -s)

ifeq ($(OS),Darwin)
	OPENGL_FLAGS=
	OPENGL_LIBS=-framework OpenGL
else
endif

MAIN_SRC=$(wildcard src/*.c)
LIB_SRC=$(wildcard lib/*.c)
GAME_SRC=$(wildcard game/*.c)

MAIN_HEADERS=$(wildcard src/*.h)
LIB_HEADERS=$(wildcard lib/*.h)
GAME_HEADERS=$(wildcard game/*.h)
#main.c shader.c load_shader.c hotload.c

SDL_FLAGS=$(shell sdl2-config --cflags)
SDL_LIBS=$(shell sdl2-config --libs)

CFLAGS=-std=c11 -pedantic -Wall -Werror -Wextra -O0 $(SDL_FLAGS) $(OPENGL_FLAGS) -framework CoreServices -pthread
LDLIBS=-ldl $(SDL_LIBS) $(OPENGL_LIBS)

all: garden-game libgame.so libsupport.so

garden-game: $(MAIN_SRC) $(MAIN_HEADERS) $(LIB_HEADERS) $(GAME_HEADERS) libsupport.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(MAIN_SRC) $(LDLIBS) -L. -lsupport

libgame.so: $(GAME_SRC) $(GAME_HEADERS) $(LIB_HEADERS) libsupport.so
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $(GAME_SRC) $(LDLIBS) -fPIC -L. -lsupport
	if pgrep garden-game; then kill -s USR1 `pgrep garden-game`; fi

libsupport.so: $(LIB_SRC) $(LIB_HEADERS)
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $(LIB_SRC) $(LDLIBS) -fPIC

test: garden-game libgame.so libsupport.so
	./$<

clean:
	$(RM) *.o *.so garden-game
