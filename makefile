OS=$(shell uname -s)

ifeq ($(OS),Darwin)
	OPENGL_FLAGS=
	OPENGL_LIBS=-framework OpenGL
else
	OPENGL_LIBS=-lgl
endif

MAIN_SRC=$(shell find src -type f -name '*.c')
LIB_SRC=$(shell find lib -type f -name '*.c')
GAME_SRC=$(shell find game -type f -name '*.c')

MAIN_HEADERS=$(shell find src -type f -name '*.h')
LIB_HEADERS=$(shell find lib -type f -name '*.h')
GAME_HEADERS=$(shell find game -type f -name '*.h')

SDL_FLAGS=$(shell sdl2-config --cflags)
SDL_LIBS=$(shell sdl2-config --libs)

CFLAGS=-std=c11 -pedantic -Wall -Werror -Wextra -O3 $(SDL_FLAGS) $(OPENGL_FLAGS) -I.
LDLIBS=-ldl $(SDL_LIBS) $(OPENGL_LIBS)

all: garden-game libgame.so libsupport.so

garden-game: $(MAIN_SRC) $(MAIN_HEADERS) $(LIB_HEADERS) $(GAME_HEADERS) libsupport.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(MAIN_SRC) $(LDLIBS) -L. -lsupport -framework CoreServices

libgame.so: $(GAME_SRC) $(GAME_HEADERS) $(LIB_HEADERS) libsupport.so
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $(GAME_SRC) $(LDLIBS) -fPIC -L. -lsupport
	if pgrep garden-game; then kill -s USR1 `pgrep garden-game`; fi

libsupport.so: $(LIB_SRC) $(LIB_HEADERS)
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $(LIB_SRC) $(LDLIBS) -fPIC

test: garden-game libgame.so libsupport.so
	./$<

clean:
	$(RM) *.o *.so garden-game
