CC = clang
CFLAGS = -std=c11 -Wall -pedantic -g
  
LFLAGS = -lSDL2 -lSDL2_ttf -lm -fsanitize=undefined

FILES = src/*.c $\
  
default: src/main.c
	$(CC) -I/usr/include/SDL2 -D_REENTRANT $(CFLAGS) -o run $(FILES) $(LFLAGS)
