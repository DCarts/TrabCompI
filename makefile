CC=gcc

CFLAGS=-g -Wall -D_REENTRANT -pedantic

LINUX_FLAGS=-D_GNU_SOURCE=1

IMAGE_FLAGS=-lSDL2_image
TTF_FLAGS=-lSDL2_ttf
AUDIO_FLAGS=-lSDL2_mixer
MATH_FLAGS=-lm

SFLAGS=-lSDL2

GAME_FLAGS= $(SFLAGS) $(IMAGE_FLAGS) $(AUDIO_FLAGS) $(TTF_FLAGS) $(MATH_FLAGS)

SOURCES=main.c global.c game.c
OBJS=main.o global.o game.o media.o render.o util.o
BINARIES=breakout lvlbuilder

#all: ex0 ex1 ex2 ex3 ex4 ex5 ex6 ex7 grafico texto
# $(SOURCES) $(CFLAGS) $(LINUX_FLAGS) $(SFLAGS) $(IMAGE_FLAGS) $(AUDIO_FLAGS) $(TTF_FLAGS)
all: $(BINARIES)

breakout: $(OBJS)
	$(CC) -o ./bin/breakout $(OBJS) $(GAME_FLAGS)

lvlbuilder: levelBuilder.c
	$(CC) -o ./bin/lvlbuilder levelBuilder.c $(CFLAGS) $(LINUX_FLAGS) $(SFLAGS) $(IMAGE_FLAGS) $(MATH_FLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

global.o: global.c
	$(CC) -c global.c $(CFLAGS) $(GAME_FLAGS)

game.o: game.c
	$(CC) -c game.c $(CFLAGS) $(GAME_FLAGS)

media.o: media.c
	$(CC) -c media.c $(CFLAGS) $(GAME_FLAGS)

render.o: render.c
	$(CC) -c render.c $(CFLAGS) $(GAME_FLAGS)

util.o: util.c
	$(CC) -c util.c $(CFLAGS) $(GAME_FLAGS)

clean:
	rm -rf *.o *.exe *.bak *.c~ $(BINARIES) core a.out

limpa:
	del *.o *.exe *.bak *.c~ $(BINARIES) core a.out

