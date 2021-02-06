#pragma once

/*! @mainpage Sycamore Documentation
 * Sycamore is a hobby game engine. It is currently in pre-alpha and not even an engine. It is a personal project that I
 * am using in order to learn a bunch of things that I am interested in.
 *
 * @section setupandbuilding Setup & Building
 * To build it, you must have CMake and a C++ compiler installed. I recommend using ninja and g++ or MSVC, as those are
 * the tools that will be used during development, clang should work but carries no guarantees. Currently only works on
 * windows, Linux & Web to come. To build the docs [Doxygen](https://www.doxygen.nl/index.html) is required.
 *
 * First clone [The reposotry](https://github.com/alexa-griffin/sycamore/) recursively.
 * Then run the setup.bat file to generate the build files.
 * ```
 * $ setup.bat <ninja|msvc>
 * ```
 *
 * If you build with ninja, a multi-config ninja build will be generated. you can then run:
 * ```
 * $ build.bat <Release|Debug|RelWithDebInfo>
 * or
 * $ run.bat <Release|Debug|RelWithDebInfo>
 * ```
 * from the /build/ninja/ directory. Which will either build, or build and run the sandbox.
 *
 * You can configure sycamore manually but it is somewhat complicated.
 *
 * @section Documentation
 * Sycamore consists of a bunch of modules (not c++20 modules) that are largely independent, if you want to do something
 * with rendering it will be in the rendering module, etc. Thus it is recommended to learn by looking through the
 * <a href="modules.html">modules</a>.
 *
 * In addition to this there is an alphabetical list of all classes <a href="classes.html">here</a> if you want a broad
 * overview of what is possible.
 *
 * @section Credits
 * - [SDL2](https://www.libsdl.org/) ([licence](https://www.libsdl.org/license.php))
 */

#include "pch.hpp"

/// @defgroup debug Debug Utilities
/// Sycamore provides several debug functionalities, because Sycamore is meant to be crossplatform, profiling and debug
/// capabilities are built in, and not reliant on any platform depdendent external tools.
#include "debug/logging.hpp"
#include "debug/profiler.hpp"

/// @defgroup maths Maths
/// Sycamore implements its own maths functions, currently it is on a "first-needed first-served" basis. So *a lot* of
/// functionality is missing. This will be changed both as time goes on and when the 1.0 release happens.
#include "maths/maths.hpp"

/// @defgroup platform Platform Abstraction Layer
/// Sycamore uses [SDL2](https://www.libsdl.org/) to handle most platform functionlaity, sycamore provides a relatively
/// thin wrapper on top of this, as well as much functionality that SDL2 does not implement.
#include "platform/platform.hpp"
#include "platform/terminal.hpp"