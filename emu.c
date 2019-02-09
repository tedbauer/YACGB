#include <stdio.h>
#include "cpu.h"
#include "mem.h"

int main() {
	mem_state_t* mem = init_mem();
	init_cpu(mem);
	while (1) {
		if (step_cpu()) {
			return -1;
		}
	}
	return 0;
}
