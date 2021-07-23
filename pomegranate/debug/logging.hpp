#pragma once

#include <iostream>

#ifdef POM_PLATFORM_WINDOWS
#    define POM_DEBUGBREAK() __debugbreak()
#endif

#include "platform/terminal.hpp"

namespace pom {
    /// @addtogroup debug
    /// @{

    /// Log levels are the specified filter for terminal output, if the current log level is lower than the specified
    /// message, then it should not be printed. For debug builds it defaults to `DEBUG` and for release builds it
    /// defaults to `ERROR`.
    enum class LogLevel {
        ALL = 0, ///< Outputs all log messages.
        TRACE = 0, ///< Outputs trace messages and above. (Currently equivelent to `ALL`)
        DEBUG = 1, ///< Output debug messages and above.
        INFO = 2, ///< Output info messages and above.
        WARN = 3, ///< Output warn messages and above.
        ERROR = 4, ///< Output error messages and above.
        FATAL = 5 ///< Output fatal messages and above.
    };

    /// Sets the current log level. This should primarily be done by the build system.
    /// @warning this code is unsafe and running it from multiple threads may cause issues.
    POM_API void setLogLevel(LogLevel level);

    /// Returns true if the specified level is active based on the current log level.
    /// @param level the level to check against, this is done by a comparison of the integer values.
    POM_API bool isLogLevelActive(LogLevel level);

    /// Logs a set of arguments to an output string, not intended to be used manually.
    /// @private
    template <typename... Args> void _log(std::ostream& os, LogLevel level, Args&&... args)
    {
        if (isLogLevelActive(level)) {
            ((os << args), ...) << std::endl;
        }
    }

    /// @]
} // namespace pom

/// @addtogroup debug
/// @{

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
/// POM_TRACE(4, " ", "strings", 4.5f);
/// // [TRACE] filename:linenumber : 4 strings4.5
/// ```
/// @{

// clang-format off

/// Logs a trace message.
#define POM_TRACE(...) ::pom::_log(::std::cout, ::pom::LogLevel::TRACE, ::pom::terminal::grey,   "[TRACE] ", ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs an info message.
#define POM_INFO(...)  ::pom::_log(::std::cout, ::pom::LogLevel::INFO,  ::pom::terminal::blue,   "[INFO] ",  ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a debug message.
#define POM_DEBUG(...) ::pom::_log(::std::cout, ::pom::LogLevel::DEBUG, ::pom::terminal::green,  "[DEBUG] ", ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a warning.
#define POM_WARN(...)  ::pom::_log(::std::cout, ::pom::LogLevel::WARN,  ::pom::terminal::yellow, "[WARN] ",  ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs an error.
#define POM_ERROR(...) ::pom::_log(::std::cout, ::pom::LogLevel::ERROR, ::pom::terminal::red,    "[ERROR] ", ::pom::terminal::reset, __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
/// Logs a fatal error and panics.
#define POM_FATAL(...)                                                                                             \
    {                                                                                                                  \
        ::pom::_log(::std::cerr,                                                                                       \
                    ::pom::LogLevel::FATAL,                                                                            \
                    ::pom::terminal::black,                                                                            \
                    ::pom::terminal::onRed,                                                                            \
                    "[FATAL]",                                                                                         \
                    ::pom::terminal::reset,                                                                            \
                    " ",                                                                                               \
                    __FILENAME__,                                                                                      \
                    ":",                                                                                               \
                    __LINE__,                                                                                          \
                    " : ",                                                                                             \
                    __VA_ARGS__);                                                                                      \
        POM_DEBUGBREAK();                                                                                              \
    }

// clang-format on

/// @}
/// @}

/// Asserts if the first argument evaluates to false. If the assert fails then it will print the
/// rest of the arguments in the same fashion described in @ref terminal_output "Terminal Output"
#define POM_ASSERT(condition, ...)                                                                                     \
    {                                                                                                                  \
        if (!(condition)) {                                                                                            \
            ::pom::_log(::std::cerr,                                                                                   \
                        ::pom::LogLevel::FATAL,                                                                        \
                        ::pom::terminal::black,                                                                        \
                        ::pom::terminal::onRed,                                                                        \
                        "[ASSERT]",                                                                                    \
                        ::pom::terminal::reset,                                                                        \
                        " ",                                                                                           \
                        __FILENAME__,                                                                                  \
                        ":",                                                                                           \
                        __LINE__,                                                                                      \
                        " : ",                                                                                         \
                        __VA_ARGS__);                                                                                  \
            POM_DEBUGBREAK();                                                                                          \
        }                                                                                                              \
    }
/// @}