#pragma once

// stl
#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <string>

#ifdef CE_PLATFORM_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include "windows.h"
#endif

#include "SDL.h"

// chione stuffs.
#include "debug/logging.hpp"
#include "debug/profiler.hpp"