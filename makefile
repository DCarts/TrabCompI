CC=gcc

CFLAGS=-g -Wall -D_REENTRANT -lm

LINUX_FLAGS=-D_GNU_SOURCE=1

IMAGE_FLAGS=-lSDL2_image
TTF_FLAGS=-lSDL2_ttf
AUDIO_FLAGS=-lSDL2_mixer
SFLAGS=-lSDL2

SOURCES=main.c
BINARIES=bin/breakout

#all: ex0 ex1 ex2 ex3 ex4 ex5 ex6 ex7 grafico texto
all: $(BINARIES)

bin/breakout: main.c
	$(CC) -o $(BINARIES) $(SOURCES) $(CFLAGS) $(LINUX_FLAGS) $(SFLAGS) $(IMAGE_FLAGS) $(AUDIO_FLAGS) $(TTF_FLAGS)
