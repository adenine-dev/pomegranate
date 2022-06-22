#pragma once

#include "base.hpp"

#include "fmt/color.h"
#include "fmt/core.h"
#include <source_location>

#ifdef POM_PLATFORM_WINDOWS
    #define POM_DEBUGBREAK() __debugbreak()
#endif

namespace pom {
    enum class LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    struct FormatLocationInfo {
        template <typename F>
        constexpr FormatLocationInfo(F f, std::source_location l = std::source_location::current()) : format(f),
                                                                                                      loc(l)
        {}

        fmt::string_view format;
        std::source_location loc;
    };

    void _log(LogLevel level, const FormatLocationInfo &locFormatInfo, fmt::format_args args);

    void setLogLevel(LogLevel level);

    template <typename... Args>
    constexpr void trace(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::TRACE, locFormatInfo, fmt::make_format_args(args...));
    }

    template <typename... Args>
    constexpr void debug(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::DEBUG, locFormatInfo, fmt::make_format_args(args...));
    }

    template <typename... Args>
    constexpr void info(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::INFO, locFormatInfo, fmt::make_format_args(args...));
    }

    template <typename... Args>
    constexpr void warn(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::WARN, locFormatInfo, fmt::make_format_args(args...));
    }

    template <typename... Args>
    constexpr void error(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::ERROR, locFormatInfo, fmt::make_format_args(args...));
    }

    template <typename... Args>
    constexpr void fatal(const FormatLocationInfo &locFormatInfo, Args &&...args)
    {
        _log(LogLevel::FATAL, locFormatInfo, fmt::make_format_args(args...));

        POM_DEBUGBREAK();
    }

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