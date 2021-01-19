# Sycamore engine

Sycamore is a hobby game engine.

To build it, you must have CMake and a C++ compiler installed. I recommend using ninja and g++ or MSVC, as those are the tools that will be used during development, clang should work but carry no guarantees. Currently only works on windows. Linux to come.

To setup clone the repo then run one set of these commands: 
```
setup.bat ninja
cd build
// build in one of these ways:
build.bat <Build type:Release|Debug|RelWithDebInfo>
run.bat <Build type:Release|Debug|RelWithDebInfo>

// or

setup.bat msvc
// then open build/msvc/Sycamore.sln
```

manual setup can be done, however for consistency and ease of use it is recommended to do it through the setup script.


## Third Party
- [SDL2](https://www.libsdl.org/) [licence](https://www.libsdl.org/license.php)