CC = gcc
CFLAGS = -g -Wall

all: run

run: emu
	./emu

emu: emu.o cpu.o mem.o
	$(CC) $(CFLAGS) -o emu emu.o cpu.o mem.o

cpu.o: cpu.c cpu.h mem.h
	$(CC) $(CFLAGS) -c cpu.c

mem.o: mem.c mem.h
	$(CC) $(CFLAGS) -c mem.c

emu.o: emu.c cpu.c cpu.h mem.c mem.h
	$(CC) $(CFLAGS) -c emu.c

clean:
	$(RM) emu *.o *~
