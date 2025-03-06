## How to compile

This repo uses [nob.h](https://github.com/tsoding/nob.h) as a build system

Firts of all you need to bootstrap the build system:
```bash
gcc nob.c -o nob
```
Then, you can simply run 
```bash
./nob
```
to compile the project, or 
```bash
./nob debug
```
if you want to add debug information and remove optimizations.