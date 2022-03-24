#pragma once

/*! @mainpage Introduction
 * Pomegranate is a hobby game engine. It is currently in pre-alpha and not even an engine. It is a
 * personal project that I am using in order to learn a bunch of things that I am interested in.
 *
 * @section setupandbuilding Setup & Building
 * To build this project, you must have [CMake](https://cmake.org/), the
 * [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/), and a C++ compiler installed. Clang gcc and msvc are all tested
 * regularly however carry no grantees for any given commit. Currently only works on windows, Linux & Web to come.
 * First clone [The reposotry](https://Pgithub.com/pairswiththymine/pomegranate/) recursively:
 * ```sh
 * git clone https://github.com/pairswiththymine/pomegranate.git --recurse-submodules
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
 * - [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross)
 ([licence](https://github.com/KhronosGroup/SPIRV-Cross/blob/master/LICENSE))
 * - [STB](https://github.com/nothings/stb) ([licence](https://github.com/nothings/stb/blob/master/LICENSE))
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

/// @defgroup util Utilities
/// @brief Little utility things that don't fit anywhere else.
#include "util/bitflags.hpp"
#include "util/misc.hpp"
#include "util/ref.hpp"

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

/// @defgroup graphics Graphics
/// @brief Graphical capabilities of the engine.
#include "graphics/color.hpp"

/// @defgroup gfx GFX
/// @brief Low level graphics api abstraction layer.
/// @ingroup graphics
#include "graphics/gfx/buffer.hpp"
#include "graphics/gfx/commandBuffer.hpp"
#include "graphics/gfx/context.hpp"
#include "graphics/gfx/descriptorSet.hpp"
#include "graphics/gfx/gfx.hpp"
#include "graphics/gfx/instance.hpp"
#include "graphics/gfx/pipeline.hpp"
#include "graphics/gfx/pipelineLayout.hpp"
#include "graphics/gfx/renderPass.hpp"
#include "graphics/gfx/shader.hpp"
#include "graphics/gfx/texture.hpp"

// TODO: docs
#include "graphics/mesh.hpp"

// TOOD: docs
#include "graphics/renderer/spriteRenderer.hpp"

// TODO: if vulkan
/// @defgroup vulkan Vulkan Implementation
/// @ingroup gfx
/// @brief Vulkan implementation of GFX
#include "graphics/gfx/vulkan/bufferVk.hpp"
#include "graphics/gfx/vulkan/commandBufferVk.hpp"
#include "graphics/gfx/vulkan/contextVk.hpp"
#include "graphics/gfx/vulkan/descriptorSetVk.hpp"
#include "graphics/gfx/vulkan/gfxVk.hpp"
#include "graphics/gfx/vulkan/instanceVk.hpp"
#include "graphics/gfx/vulkan/pipelineLayoutVk.hpp"
#include "graphics/gfx/vulkan/pipelineVk.hpp"
#include "graphics/gfx/vulkan/renderPassVk.hpp"
#include "graphics/gfx/vulkan/shaderVk.hpp"
#include "graphics/gfx/vulkan/textureVk.hpp"
#include "graphics/gfx/vulkan/typesVk.hpp"

/// @defgroup scene Scene

/// @defgroup ecs Entity Component System
/// @ingroup scene
/// @brief Contains the underlying logic for entity management.
/// @details Pomegranate uses a custom archetype based entity component system. An entity component system or ECS is a
/// method of organizing game objects (entities) in such a way that they each have a set of components which contain
/// their data, and can be acted on individually or in groups. \n
/// In this engine components are defined to be trivially copyable and destructable structures. By convention they are
/// also zero constructable and all their data is public.
#include "scene/ecs/archetype.hpp"
#include "scene/ecs/component.hpp"
#include "scene/ecs/store.hpp"
#include "scene/ecs/type.hpp"
#include "scene/ecs/view.hpp"

/// @defgroup core Core
/// @brief Core functionality.
#include "core/application.hpp"
#include "core/clientInterface.hpp"
#include "core/core.hpp"
#include "core/timing.hpp"
