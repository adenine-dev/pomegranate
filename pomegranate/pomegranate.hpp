#pragma once

/*! @mainpage Introduction
 * Pomegranate is a hobby game engine. It is currently in pre-alpha and not even an engine. It is a
 * personal project that I am using in order to learn a bunch of things that I am interested in.
 *
 * @section setupandbuilding Setup & Building
 * To build this project, you must have [CMake](https://cmake.org/), the
 * [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/), and a C++ compiler installed. Clang gcc and msvc are all tested
 * regularly however carry no grantees for any given commit. Currently only works on windows, Linux & Web to come.
 * First clone [The reposotry](https://Pgithub.com/alexa-griffin/pomegranate/) recursively:
 * ```sh
 * git clone https://github.com/alexa-griffin/pomegranate.git --recurse-submodules
 * ```
 * Then setup CMake however you want, it should look something like:
 * ```sh
 * mkdir build
 * cd build
 * cmake .. -DCMAKE_BUILD_TYPE=Debug -G "generator"
 * ```
 * finally build, which should look something like:
 * ```
 * cmake --build . --config Debug --target pom_runtime -j 10
 * ```
 * The final executable will be in `/bin/Debug/`.
 * If you are lazy (like me) the vscode
 * [CMake tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) is useful.
 * @section Documentation
 * Pomegranate consists of a bunch of pieces that are largely independent, if
 * you want to do something with rendering it will be in the rendering module, etc. Thus it is
 * recommended to learn by looking through the <a href="modules.html">modules</a>.
 *
 * In addition to this there is an alphabetical list of all classes <a href="classes.html">here</a>
 * if you want a broad overview of what is currently possible.
 *
 * To build the docs, install [Doxygen](https://www.doxygen.nl/index.html), then run the CMsake docs target after
 * configuring.
 *
 * @section Dependencies
 * - [CMake](https://cmake.org/)
 * - [Doxygen](https://www.doxygen.nl/index.html)
 * - [SDL2](https://www.libsdl.org/) ([licence](https://www.libsdl.org/license.php))
 * - [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
 */

#include "pch.hpp"

/// @defgroup debug Debug Utilities
/// @brief Various debugging tools built into Pomegranate.
///
/// Pomegranate provides several debug functionalities, because Pomegranate is meant to be
/// crossplatform, profiling and debug capabilities are built in, and not reliant on any platform
/// depdendent external tools.
#include "debug/logging.hpp"
#include "debug/profiler.hpp"

/// @defgroup maths Maths
/// @brief All mathematical functionality.
///
/// Pomegranate implements its own maths functions, currently it is on a "first-needed first-served"
/// basis. So *a lot* of functionality is missing. This will be changed both as time goes on and
/// when the 1.0 release happens. All basic types are of the format `t##` where `t` is the type
/// abbreviation (`u` for unsigned int, `f` for floating point, etc) and `##` is the size in bits.
///
/// `#include <pomegranate/maths/maths.hpp>`
#include "maths/maths.hpp"

/// @defgroup platform Platform
/// @brief Platform abstracitons.
///
/// Pomegranate uses [SDL2](https://www.libsdl.org/) to handle most platform interaction,
/// Pomegranate provides a relatively thin wrapper on top of this, as well as some functionality
/// that SDL2 does not implement.
#include "platform/input.hpp"
#include "platform/platform.hpp"
#include "platform/terminal.hpp"
#include "platform/window.hpp"

/// @defgroup core Core
/// @brief Core functionality.
#include "core/application.hpp"
