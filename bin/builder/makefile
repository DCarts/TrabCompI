CC=gcc

CFLAGS=-g -Wall -D_REENTRANT -lm

LINUX_FLAGS=-D_GNU_SOURCE=1

IMAGE_FLAGS=-lSDL2_image
AUDIO_FLAGS=-lSDL2_mixer
SFLAGS=-lSDL2

SOURCES=levelBuilder.c
BINARIES=lvlbuilder

#all: ex0 ex1 ex2 ex3 ex4 ex5 ex6 ex7 grafico texto
all: $(BINARIES)

lvlbuilder: levelBuilder.c
	$(CC) -o ../$(BINARIES) $(SOURCES) $(CFLAGS) $(LINUX_FLAGS) $(SFLAGS) $(IMAGE_FLAGS) $(AUDIO_FLAGS)
