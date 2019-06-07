#include <stdio.h>
#include <stdlib.h>

typedef char * rom_t;

rom_t* open_rom(const char *rom_fpath)
{
	int rom_size = 0;
	FILE* rom_file = fopen(rom_fpath, "r");
	while (fgetc(rom_file) != EOF)
		rom_size++;
	rom_t* rom = malloc(rom_size);
	int i = 0;
	fseek(rom_file, 0, SEEK_SET);
	while (fread(rom[i], 1, 1, rom_file) == 1)
		i++;
	fclose(rom_file);
	return rom;
}

int read_rom(const rom_t rom, int offset)
{
	return rom[offset];
}

void close_rom(const rom_t rom)
{
	free(rom);
}
