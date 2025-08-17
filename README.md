<!-- vim: set tw=80 cc=80: -->
# Z80-Retro! Snake Game

This game is different to the earlier version released as part of the Nabu-Games
collection.

## Why

This version uses the TMS9118A on the Z80Retro in Multicolor mode.  Multicolor
mode is not very popular and there is almost no content available for it other
than a few demo examples.  However, when I was thinking about games like
"Snake", "Conways Game Of Life" and the "Chip-8" emulator, I thought that the
multicolor mode might work well with them.

## How

All the other games I have written in the past for the Z80-Retro (or the Nabu)
have been either written in raw assembly and assmebled / compiled by the
following tools:

- z80asm standard package on Debian based distros
- Z88DK full featured retro C compiler, assembler and linker for z80 based code.

This time, round, I decided to develop with the "Fuzix-Compiler-Kit" C compiler
and "Fuzix-Bintools" assembler and linker.

## Build Instructions

- Get Install the Fuzix-Compiler-Kit and Fuzix-Bintools on your system.
- Clone this project
``` bash
cd z80retro-snake
make world
```

Then use the `XR.COM` application on the Z80Retro to copy the `main.com`
application onto your SDCARD.


