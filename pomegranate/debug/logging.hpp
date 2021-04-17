#pragma once

#include <iostream>

#ifdef POM_PLATFORM_WINDOWS
#    define POM_DEBUGBREAK() __debugbreak()
#endif

#include "platform/terminal.hpp"

/// @addtogroup debug
/// @{
namespace pom {
    /// Logs a set of arguments to an output string, not intended to be used manually.
    /// @private
    template <typename... Args> void _log(std::ostream& os, Args&&... args)
    {
        ((os << args), ...) << std::endl;
    }
} // namespace pom

/// @addtogroup terminal_output Terminal Output
/// @brief Tools for interacting with the output console.
/// @{

/// @anchoredname{Debug Output, debug_output}
/// Programs use the terminal rather extensively, and Pomegranate provides its own functions for
/// interacting with it. The `POM_LOG_*` macros will output a consistently formatted stream of data
/// to the console. There is no formatting here, you simply pass the arguments to the macro and it
/// prints all of them consecutively. If you wish to color your output then you can use the @ref
/// terminal_styles "terminal styling functions". For example:
/// ```cpp
/// POM_LOG_TRACE(4, " ", "strings", 4.5f);
/// // [TRACE] filename:linenumber : 4 strings4.5
/// ```
/// @{

// clang-format off

/// Logs a trace message, currently the log levels are purely ornamental.
#define POM_LOG_TRACE(...) ::pom::_log(::std::cout, ::pom::terminal::grey,   "[TRACE] ", ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs an info message, currently the log levels are purely ornamental.
#define POM_LOG_INFO(...)  ::pom::_log(::std::cout, ::pom::terminal::blue,   "[INFO] ",  ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a debug message, currently the log levels are purely ornamental.
#define POM_LOG_DEBUG(...) ::pom::_log(::std::cout, ::pom::terminal::green,  "[DEBUG] ", ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a warning, currently the log levels are purely ornamental.
#define POM_LOG_WARN(...)  ::pom::_log(::std::cout, ::pom::terminal::yellow, "[WARN] ",  ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs an error, currently the log levels are purely ornamental.
#define POM_LOG_ERROR(...) ::pom::_log(::std::cout, ::pom::terminal::red,    "[ERROR] ", ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)

// clang-format on

/// @}
/// @}

/// Asserts if the first argument evaluates to not true. If the assert fails then it will print the
/// rest of the arguments in the same fashion described in @ref terminal_output "Terminal Output"
#define POM_ASSERT(condition, ...)                                                                 \
    {                                                                                              \
        if (!(condition)) {                                                                        \
            ::pom::_log(::std::cerr,                                                               \
                        ::pom::terminal::black,                                                    \
                        ::pom::terminal::onRed,                                                    \
                        "[ASSERT]",                                                                \
                        ::pom::terminal::reset,                                                    \
                        " ",                                                                       \
                        __FILENAME__,                                                              \
                        ":",                                                                       \
                        __LINE__,                                                                  \
                        " : ",                                                                     \
                        __VA_ARGS__);                                                              \
            POM_DEBUGBREAK();                                                                      \
        }                                                                                          \
    }

/// @}