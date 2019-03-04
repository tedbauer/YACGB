# Yacgb

Yet another C GB emulator! Highly WIP.

## Building

Yacgb requires [SDL2][sdl]. If you're on a Mac, I recommend installing
it with [Homebrew][brew] by typing `brew install sdl2`. Then, you can:
- `make` to build & run
- `make debug` to build & run with debugging output
- `make emu` to create binary executable `emu`
- `make clean` to clean up build stuff

[sdl]: https://www.libsdl.org/download-2.0.php
[brew]: https://brew.sh/

## Tasks
- [ ] CPU
	- [X] BIOS instructions
	- [ ] The rest!
- [X] Memory
- [ ] ROM parser
- [ ] Graphics subsystem
- [ ] Fancy debugger
- [ ] Game Link Cable
- [ ] Other stuff

## Helpful Resources
- [Imran Nazar's GB emulator resources][1]
- [Tomek's GB emulation blog post][2]
- [Game Boy CPU manual][3]
- [Game Boy CPU instruction set guide][4]
- [z80 Heaven][5]
- [z80.info][6]
- [Gameboy BIOS][7]

[1]: http://imrannazar.com/GameBoy-Emulation-in-JavaScript
[2]: https://blog.rekawek.eu/2017/02/09/coffee-gb/
[3]: http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
[4]: http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
[5]: http://z80-heaven.wikidot.com/
[6]: http://www.z80.info/
[7]: http://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM
