#pragma once

#include "maths/types.hpp"

#ifdef POM_PLATFORM_WINDOWS
#    define POM_EXPORT extern "C" __declspec(dllexport)
#else
#    define POM_EXPORT extern "C"
#endif

#define POM_NOCOPY(classname)                                                                                          \
    classname(const classname&) = delete;                                                                              \
    classname& operator=(const classname&) = delete;

#define POM_NOMOVE(classname)                                                                                          \
    classname(classname&&) = delete;                                                                                   \
    classname& operator=(classname&&) = delete;
