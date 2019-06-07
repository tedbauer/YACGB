#include <stdio.h>
#include <stdlib.h>
#include "read_rom.h"

int main()
{
	rom_t r = open_rom("bios");
	printf("file opened!\n");
	int* buff = malloc(sizeof(int));
	while (read_rom(r, buff)) {
		printf("%d\n", *buff);
	}
	free(buff);
	close_rom(r);
	return 0;
}
