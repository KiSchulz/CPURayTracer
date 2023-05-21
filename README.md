# CPURayTracer 
__________________
## Description
__________________
A simple cpu ray tracer.

## Dependencies
_________________
There a four dependencies.
1. Eigen
2. Open Asset Import Library
3. stb_image
4. olcPixelGameEngine

On arch-based systems the following commands can be used to install the first two dependencies.
```bash
pacman -S eigen assimp
```
The third dependency is included in the repository under stb and the olcPixelGameEngine is a submodule which can be cloned later an before compilation.

## Compilation
_______________
First clone the repository. Next use the following commands:
```bash
cd CPURayTracer
#initializes the olcPixelGameEngine folder
git submodule init
git submodule update

mkdir build
cd build
cmake ..
make all
```

## Usage
__________________
To change what is rendered use the Engine.h file.
