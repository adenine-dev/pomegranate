#pragma once

#include "maths/types.hpp"

#ifdef POM_PLATFORM_WINDOWS
#    define POM_EXPORT extern "C" __declspec(dllexport)
#else
#    define POM_EXPORT extern "C"
#endif