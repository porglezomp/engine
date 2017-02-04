OS=$(shell uname -s)

ifeq ($(OS),Darwin)
	OPENGL_FLAGS=
	OPENGL_LIBS=-framework OpenGL
else
endif

MAIN_SRC=main.c shader.c load_shader.c hotload.c

SDL_FLAGS=$(shell sdl2-config --cflags)
SDL_LIBS=$(shell sdl2-config --libs)

CFLAGS=-std=c11 -pedantic -Wall -Werror -Wextra -O0 $(SDL_FLAGS) $(OPENGL_FLAGS) -g -framework CoreServices -pthread
LDLIBS=-ldl $(SDL_LIBS) $(OPENGL_LIBS)

all: garden-game libgame.so

garden-game: $(MAIN_SRC) game.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(MAIN_SRC) $(LDLIBS)

libgame.so: game.c game.h
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $< $(LDLIBS) -fPIC
	if pgrep garden-game; then kill -s USR1 `pgrep garden-game`; fi

test: garden-game libgame.so
	./$<

clean:
	$(RM) *.o *.so garden-game
