.PHONY: all run clean

IDIR = ../include
ODIR = _build/obj

_DEPS = screen.h gpu.h cpu.h mem.h
DEPS = $(patsubst %,$(IDIR)/%, $(_DEPS))

_OBJ = screen.o gpu.o cpu.o mem.o emu.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CC = gcc
CFLAGS = -g -Wall
SDLFLAGS = $(shell sdl2-config --libs --cflags)

all: run

debug: CFLAGS += -DDEBUG
debug: run

run: emu
	./emu

emu: $(OBJ)
	$(CC) $(SDLFLAGS) $(CFLAGS) -o $@ $^

$(ODIR)/%.o: %.c $(DEPS)
	@mkdir -p $(@D)
	cc -c -o $@ $< $(CFLAGS)

clean:
	$(RM) -r emu _build *~
