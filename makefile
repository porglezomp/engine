CFLAGS=-std=c11 -pedantic -Wall -Werror -Wextra -fPIC -O3 $(sdl2-config --cflags)
LDLIBS=-ldl $(sdl2-config --libs)

all: garden-game libgame.so

garden-game: main.c game.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

libgame.so: game.c game.h
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ $< $(LDLIBS)
	kill -s USR1 `pgrep garden-game`

test: garden-game libgame.so
	./$<

clean:
	$(RM) *.o *.so garden-game 
