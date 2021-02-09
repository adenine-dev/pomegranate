#pragma once

#include <iostream>

#include "platform/platform.hpp"
#include "platform/terminal.hpp"

/// @addtogroup debug
/// @{
namespace sm {
    /// Logs a set of arguments to an output string, not intended to be used manually.
    template <typename... Args> void _log(std::ostream& os, Args&&... args)
    {
        ((os << args), ...) << std::endl;
    }
} // namespace sm

/// @defgroup terminal_output Terminal Output
/// Programs use the terminal rather extensively, and sycamore provides its own functions for interacting with it. The
/// `SM_LOG_*` macros will output a consistently formatted stream of data to the console. There is no formatting here,
/// you simply pass the arguments to the macro and it prints all of them consecutively. If you wish to color your output
/// then there are the color functions below.
/// @{
// clang-format off
#define SM_LOG_TRACE(...) ::sm::_log(::std::cout, ::sm::terminal::grey,   "[TRACE] ", ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_INFO(...)  ::sm::_log(::std::cout, ::sm::terminal::blue,   "[INFO] ",  ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_DEBUG(...) ::sm::_log(::std::cout, ::sm::terminal::green,  "[DEBUG] ", ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_WARN(...)  ::sm::_log(::std::cout, ::sm::terminal::yellow, "[WARN] ",  ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_ERROR(...) ::sm::_log(::std::cout, ::sm::terminal::red,    "[ERROR] ", ::sm::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
// clang-format on
/// @}

/// Asserts if the first argument evaluates to not true. If the assert fails then it will print the rest of the
/// arguments in the same fashion described in @ref terminal_output "Terminal Output"
#define SM_ASSERT(condition, ...)                                                                                      \
    {                                                                                                                  \
        if (!(condition)) {                                                                                            \
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

/// @}