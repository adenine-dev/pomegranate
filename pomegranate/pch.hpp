#pragma once

// stl
#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <string>

#ifdef POM_PLATFORM_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include "windows.h"

// Used in input.hpp as a names, prob won't break anything.
#    undef KEY_EXECUTE
#    undef OUT
#    undef DELETE
#endif

// libs
#include "SDL.h"

// TODO: only if vulkan
#include "SDL_vulkan.h"
#include <vulkan/vulkan.h>

// pomegranate
#include "maths/types.hpp"

#include "debug/logging.hpp"
#include "debug/profiler.hpp"