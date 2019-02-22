.PHONY: all run clean

CC = gcc
CFLAGS = -g -Wall
SDLFLAGS = $(shell sdl2-config --libs --cflags)

all: run

debug: CFLAGS += -DDEBUG
debug: run

run: emu
	./emu

emu: emu.o cpu.o mem.o screen.o
	$(CC) $(SDLFLAGS) $(CFLAGS) -o emu emu.o cpu.o mem.o screen.o

screen.o: screen.c screen.h
	$(CC) $(SDLFLAGS) -c screen.c


cpu.o: cpu.c cpu.h mem.c mem.h
	$(CC) $(CFLAGS) -c cpu.c

mem.o: mem.c mem.h
	$(CC) $(CFLAGS) -c mem.c

emu.o: emu.c cpu.c cpu.h mem.c mem.h
	$(CC) $(CFLAGS) -c emu.c

clean:
	$(RM) emu *.o *~
