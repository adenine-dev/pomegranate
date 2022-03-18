#include "logging.hpp"

namespace pom {
    namespace {
#ifdef POM_DEBUG_SYMBOLS
        LogLevel currentLogLevel = LogLevel::DEBUG;
#else
        LogLevel currentLogLevel = LogLevel::ERROR;
#endif
    } // namespace

    void setLogLevel(LogLevel level)
    {
        currentLogLevel = level;
    }

    bool isLogLevelActive(LogLevel level)
    {
        return (int)level >= (int)currentLogLevel;
    }
} // namespace pom