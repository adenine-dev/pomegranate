#pragma once

#include <iostream>

#ifdef CE_PLATFORM_WINDOWS
#    define CE_DEBUGBREAK() __debugbreak()
#endif

#include "platform/terminal.hpp"

/// @addtogroup debug
/// @{
namespace ce {
    /// Logs a set of arguments to an output string, not intended to be used manually.
    /// @private
    template <typename... Args> void _log(std::ostream& os, Args&&... args)
    {
        ((os << args), ...) << std::endl;
    }
} // namespace ce

/// @addtogroup terminal_output Terminal Output
/// @brief Tools for interacting with the output console.
/// @{

/// @anchoredname{Debug Output, debug_output}
/// Programs use the terminal rather extensively, and Chione provides its own functions for interacting with it. The
/// `CE_LOG_*` macros will output a consistently formatted stream of data to the console. There is no formatting here,
/// you simply pass the arguments to the macro and it prints all of them consecutively. If you wish to color your output
/// then you can use the @ref terminal_styles "terminal styling functions". For example:
/// ```cpp
/// CE_LOG_TRACE(4, " ", "strings", 4.5f);
/// // [TRACE] filename:linenumber : 4 strings4.5
/// ```
/// @{

// clang-format off

/// Logs a trace message, currently the log levels are purely ornamental.
#define CE_LOG_TRACE(...) ::ce::_log(::std::cout, ::ce::terminal::grey,   "[TRACE] ", ::ce::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs an info message, currently the log levels are purely ornamental.
#define CE_LOG_INFO(...)  ::ce::_log(::std::cout, ::ce::terminal::blue,   "[INFO] ",  ::ce::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a debug message, currently the log levels are purely ornamental.
#define CE_LOG_DEBUG(...) ::ce::_log(::std::cout, ::ce::terminal::green,  "[DEBUG] ", ::ce::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a warning, currently the log levels are purely ornamental.
#define CE_LOG_WARN(...)  ::ce::_log(::std::cout, ::ce::terminal::yellow, "[WARN] ",  ::ce::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs an error, currently the log levels are purely ornamental.
#define CE_LOG_ERROR(...) ::ce::_log(::std::cout, ::ce::terminal::red,    "[ERROR] ", ::ce::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)

// clang-format on

/// @}
/// @}

/// Asserts if the first argument evaluates to not true. If the assert fails then it will print the rest of the
/// arguments in the same fashion described in @ref terminal_output "Terminal Output"
#define CE_ASSERT(condition, ...)                                                                                      \
    {                                                                                                                  \
        if (!(condition)) {                                                                                            \
            ::ce::_log(::std::cerr,                                                                                    \
                       ::ce::terminal::black,                                                                          \
                       ::ce::terminal::onRed,                                                                          \
                       "[ASSERT]",                                                                                     \
                       ::ce::terminal::reset,                                                                          \
                       " ",                                                                                            \
                       __FILENAME__,                                                                                   \
                       ":",                                                                                            \
                       __LINE__,                                                                                       \
                       " : ",                                                                                          \
                       __VA_ARGS__);                                                                                   \
            CE_DEBUGBREAK();                                                                                           \
        }                                                                                                              \
    }

/// @}