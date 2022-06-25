#pragma once

#include "base.hpp"

#include "fmt/color.h"
#include "fmt/core.h"
#include <source_location>

#ifdef POM_PLATFORM_WINDOWS
    #define POM_DEBUGBREAK() __debugbreak()
#endif

/// Pom namespace
namespace pom {
    /// The log filter level, levels lower than that set by setLogLevel(LogLevel) will not be shown.
    enum class LogLevel {
        TRACE = 0, ///< Trace (All logs will be shown)
        DEBUG = 1, ///< Debug (Default log level if setLogLevel(LogLevel) is not called)
        INFO = 2,  ///< Info
        WARN = 3,  ///< Warn
        ERROR = 4, ///< Error
        FATAL = 5  ///< Fatal messages will be shown only.
    };

    /// A hacky structure that captures source_location data when constructed, this is used to allow for the unspecified
    /// capture of caller location and use a parameter pack.
    struct FormatLocationInfo {
        template <typename F>
        constexpr FormatLocationInfo(F f, std::source_location l = std::source_location::current()) : format(f),
                                                                                                      loc(l)
        {}

        /// @private
        fmt::string_view format;

        /// @private
        std::source_location loc;
    };

    /// Does the actual logging. Should not be used directly.
    void _log(LogLevel level, const FormatLocationInfo &locFormatInfo, fmt::format_args args);

    /// Sets the internal log level by which log messages will be tested against. By default this is LogLevel::DEBUG.
    void setLogLevel(LogLevel level);

    /// Logs a trace message to the console. By default these messages will not print.
    template <typename... Args>
    constexpr void trace(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::TRACE, locFormatInfo, fmt::make_format_args(args...));
    }

    /// Logs a debug message to the console.
    template <typename... Args>
    constexpr void debug(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::DEBUG, locFormatInfo, fmt::make_format_args(args...));
    }

    /// Logs an info message to the console.
    template <typename... Args>
    constexpr void info(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::INFO, locFormatInfo, fmt::make_format_args(args...));
    }

    /// Logs a warning message to the console.
    template <typename... Args>
    constexpr void warn(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::WARN, locFormatInfo, fmt::make_format_args(args...));
    }

    /// Logs an error message to the console.
    template <typename... Args>
    constexpr void error(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::ERROR, locFormatInfo, fmt::make_format_args(args...));
    }

    /// Logs a fatal message to the console, then kills the program.
    template <typename... Args>
    constexpr void fatal(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::FATAL, locFormatInfo, fmt::make_format_args(args...));

        POM_DEBUGBREAK();
    }

    /// Asserts that the passed condition is `true`, if not it prints the message then kills the program.
    template <typename... Args>
    constexpr void assert(bool condition, const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        if (!condition) {
            _log(LogLevel::FATAL, locFormatInfo, fmt::make_format_args(args...));

            POM_DEBUGBREAK();
        }
    }

} // namespace pom

template <>
struct fmt::formatter<pom::LogLevel> {
    char highlight = 0;

    constexpr auto parse(format_parse_context &ctx)
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it == 'h')
            highlight = *it++;

        if (it != end && *it != '}')
            throw format_error("invalid format");

        return it;
    }

    template <typename FormatContext>
    auto format(const pom::LogLevel &level, FormatContext &ctx) -> decltype(ctx.out())
    {
        if (highlight) {
            switch (level) {
            case pom::LogLevel::TRACE:
                return fmt::format_to(ctx.out(), fmt::emphasis::bold | fg(fmt::color::gray), "[TRACE]");
            case pom::LogLevel::DEBUG:
                return fmt::format_to(ctx.out(), fmt::emphasis::bold | fg(fmt::color::green), "[DEBUG]");
            case pom::LogLevel::INFO:
                return fmt::format_to(ctx.out(), fmt::emphasis::bold | fg(fmt::color::blue), "[INFO]");
            case pom::LogLevel::WARN:
                return fmt::format_to(ctx.out(), fmt::emphasis::bold | fg(fmt::color::yellow), "[WARN]");
            case pom::LogLevel::ERROR:
                return fmt::format_to(ctx.out(), fmt::emphasis::bold | fg(fmt::color::red), "[ERROR]");
            case pom::LogLevel::FATAL:
                return fmt::format_to(ctx.out(), fmt::emphasis::bold | bg(fmt::color::red), "[FATAL]");
            }
        } else {
            switch (level) {
            case pom::LogLevel::TRACE: return fmt::format_to(ctx.out(), "LogLevel::TRACE");
            case pom::LogLevel::DEBUG: return fmt::format_to(ctx.out(), "LogLevel::DEBUG");
            case pom::LogLevel::INFO: return fmt::format_to(ctx.out(), "LogLevel::INFO");
            case pom::LogLevel::WARN: return fmt::format_to(ctx.out(), "LogLevel::WARN");
            case pom::LogLevel::ERROR: return fmt::format_to(ctx.out(), "LogLevel::ERROR");
            case pom::LogLevel::FATAL: return fmt::format_to(ctx.out(), "LogLevel::FATAL");
            }
        }
        return fmt::format_to(ctx.out(), "unknown LogLevel");
    }
};