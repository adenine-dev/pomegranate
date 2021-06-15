# Pomegranate engine

Pomegranate is a hobby game engine.
Pomegranate is a hobby game engine. It is currently in pre-alpha and not even an engine. It is a
personal project that I am using in order to learn a bunch of things that I am interested in.

@section setupandbuilding Setup & Building
To build this project, you must have [CMake](https://cmake.org/), the 
[Vulkan SDK](https://www.lunarg.com/vulkan-sdk/), and a C++ compiler installed. Clang gcc and msvc are all tested 
regularly however carry no grantees for any given commit. Currently only works on windows, Linux & Web to come.
First clone [The reposotry](https://Pgithub.com/alexa-griffin/pomegranate/) recursively:
```sh
git clone https://github.com/alexa-griffin/pomegranate.git --recurse-submodules
```
Then setup CMake however you want, it should look something like:
```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -G "generator"
```
finally build, which should look something like:
```
cmake --build . --config Debug --target pom_runtime -j 10
```
The final executable will be in `/bin/Debug/`.
If you are lazy (like me) the vscode 
[CMake tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) is useful.


## Third Party
- [SDL2](https://www.libsdl.org/) [licence](https://www.libsdl.org/license.php)