#include "pch.hpp"

#include "profiler.hpp"

#include "logging.hpp"

namespace pom {
    Profiler::Profiler(const char* function, const char* file, u64 line) :
        functionName(function), fileName(file), lineNumber(line), startTime(std::chrono::high_resolution_clock::now())
    {
    }

    Profiler::~Profiler()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = endTime - startTime;

        // follow the same format as the other logging functions for now.
        // TODO: replace this with file stuffs and speedscope
        _log(std::cout,
             terminal::magenta,
             "[PROFILER] ",
             terminal::reset,
             fileName,
             ":",
             lineNumber,
             "[",
             functionName,
             "] : ",
             std::chrono::duration_cast<std::chrono::microseconds>(duration).count(),
             "µs");
    }
} // namespace pom