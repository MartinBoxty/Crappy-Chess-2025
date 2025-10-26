# Crappy Chess
This is a crappy Chess game that I am currently writing for two purposes:
- To learn SDL3
- I hate the unfinished OOP slop attempt at a chess game that I did in my first year at Uni and want to write it in C instead

This is not something that is intended to be "published", and this public repository is mostly just for record-keeping. Feel free to read the source code to get a feel for what my coding style looks like when I'm not beholden to other people's opinions.

If you have somehow stumbled onto this of your own accord and wish to download and mess around with the unfinished game yourself, please follow the instructions below.

## Build Instructions
### Dependencies:
- **SDL 3.2.20:** https://github.com/libsdl-org/SDL/releases/tag/release-3.2.20
- **SDL_image 3.2.4:** https://github.com/libsdl-org/SDL_image/releases/tag/release-3.2.4

### C standard:
C11

### Compiler:
GCC 15.2.0

## Other info:
I am initially just working on building out the game first and foremost. UI and functionality that doesn't pertain to the game itself may be added at a later date but is low priority.

Testing is done mostly manually by brute force by making ephemeral additions/modifications to initial game state and seeing what clicking different things in different orders achieves.

Application testing is done on Windows 10 with the following hardware specs:
- **Processor:** AMD Ryzen 9 5900 12-Core Processor 3.70 GHz
- **Installed RAM:** 32.0 GB
- **Graphics Card:** NVIDEA GeForce RTX 3080 (10 GB)
- **System Type:** 64-bit operating system, x64-based processor

I do not intend to test this on any other hardware/operating systems. Given that it is SDL and largely-standard C, it should be reasonably portable if you want to build for other targets.