#include "logging.hpp"

namespace pom {
    namespace {
#ifdef NDEBUG
        LogLevel currentLogLevel = LogLevel::ERROR;
#else
        LogLevel currentLogLevel = LogLevel::DEBUG;
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