# Yacgb

Yet another C GB emulator! Highly WIP.

## Building

Yacgb requires [SDL2](https://www.libsdl.org/download-2.0.php). If you're on a Mac, I recommend installing it with [Homebrew](https://brew.sh/) by typing `brew install sdl2`. Then, you can:
- `make` to build & run
- `make debug` to build & run with debugging output
- `make emu` to create binary executable `emu`
- `make clean` to clean up build stuff

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
- [Imran Nazar's GB emulator resources](http://imrannazar.com/GameBoy-Emulation-in-JavaScript)
- [Tomek's GB emulation blog post](https://blog.rekawek.eu/2017/02/09/coffee-gb/)
- [Game Boy CPU manual](http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf)
- [Game Boy CPU instruction set guide](http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)
- [z80 Heaven](http://z80-heaven.wikidot.com/)
- [z80.info](http://www.z80.info/)
- [Gameboy BIOS](http://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM)
