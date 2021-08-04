#pragma once

// stl
#include <chrono>
#include <cmath>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <ratio>
#include <string>
#include <utility>

// libs
#include "SDL.h"

// TODO: only if vulkan
#include "SDL_vulkan.h"
#include <vulkan/vulkan.h>

// pomegranate
#include "base.hpp"

#include "debug/logging.hpp"
#include "debug/profiler.hpp"