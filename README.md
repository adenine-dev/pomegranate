# Sycamore engine

Sycamore is a hobby game engine.

To build it, you must have CMake and a C++ compiler installed. I recommend using ninja and g++, as those are the tools that will be used during development, MSVC and clang should work but carry no guarantees. Currently only works on windows. Linux to come.

To setup clone the repo then run these commands: 
```
mkdir build
cd build
cmake -GNinja ..
ninja -j20 && "../bin/sandbox.exe"
```