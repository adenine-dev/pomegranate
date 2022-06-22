#include "logging.hpp"

#include "base.hpp"

namespace pom {
    LogLevel currentLogLevel = LogLevel::DEBUG;

    void _log(LogLevel level, const FormatLocationInfo &locFormatInfo, fmt::format_args args)
    {
        if (level >= currentLogLevel)
            fmt::print("{}:{} {:h}: {}\n",
                       locFormatInfo.loc.file_name(),
                       locFormatInfo.loc.line(),
                       level,
                       fmt::vformat(locFormatInfo.format, args));
    }

    void setLogLevel(LogLevel level)
    {
        currentLogLevel = level;
    }
} // namespace pom