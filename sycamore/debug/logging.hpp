#pragma once

#include <iostream>

namespace sm {
    template <typename... Args> void _log(std::ostream& os, Args&&... args)
    {
        ((os << args), ...) << std::endl;
    }
} // namespace sm

#define SM_LOG_TRACE(...) ::sm::_log(::std::cout, "[TRACE] ", __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_INFO(...) ::sm::_log(::std::cout, "[INFO] ", __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_DEBUG(...) ::sm::_log(::std::cout, "[DEBUG] ", __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_WARN(...) ::sm::_log(::std::cout, "[WARN] ", __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
#define SM_LOG_ERROR(...) ::sm::_log(::std::cout, "[ERROR] ", __FILENAME__, ":", __LINE__, " : ", __VA_ARGS__)
