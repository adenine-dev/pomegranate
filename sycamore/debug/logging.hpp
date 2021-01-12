#pragma once

#include <iostream>

#include "platform/platform.hpp"
#include "platform/terminal.hpp"

namespace sm {
    template <typename... Args> void _log(std::ostream& os, Args&&... args)
    {
        ((os << args), ...) << std::endl;
    }
} // namespace sm

// clang-format off

#define SM_LOG_TRACE(...) ::sm::_log(::std::cout, ::sm::terminal::grey,   "[TRACE] ", ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_INFO(...)  ::sm::_log(::std::cout, ::sm::terminal::blue,   "[INFO] ",  ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_DEBUG(...) ::sm::_log(::std::cout, ::sm::terminal::green,  "[DEBUG] ", ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_WARN(...)  ::sm::_log(::std::cout, ::sm::terminal::yellow, "[WARN] ",  ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_ERROR(...) ::sm::_log(::std::cout, ::sm::terminal::red,    "[ERROR] ", ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)

// clang-format on

#define SM_ASSERT(test, ...)                                                                                           \
    {                                                                                                                  \
        if (!(test)) {                                                                                                 \
            ::sm::_log(::std::cerr,                                                                                    \
                       ::sm::terminal::black,                                                                          \
                       ::sm::terminal::onRed,                                                                          \
                       "[ASSERT]",                                                                                     \
                       ::sm::terminal::reset,                                                                          \
                       " ",                                                                                            \
                       __FILENAME__,                                                                                   \
                       ":",                                                                                            \
                       __LINE__,                                                                                       \
                       " : ",                                                                                          \
                       __VA_ARGS__);                                                                                   \
            SM_DEBUGBREAK();                                                                                           \
        }                                                                                                              \
    }
