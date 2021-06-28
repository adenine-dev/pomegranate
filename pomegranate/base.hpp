#pragma once

#include "maths/types.hpp"

#ifdef POM_PLATFORM_WINDOWS
#    define POM_CLIENT_EXPORT extern "C" __declspec(dllexport)
#else
#    define POM_CLIENT_EXPORT extern "C"
#endif

#ifdef POM_PLATFORM_WINDOWS
#    ifdef POM_DLL
#        define POM_API __declspec(dllexport)
#    else
#        define POM_API __declspec(dllimport)
#    endif
#endif

#define POM_NOCOPY(classname)                                                                                          \
    classname(const classname&) = delete;                                                                              \
    classname& operator=(const classname&) = delete;
