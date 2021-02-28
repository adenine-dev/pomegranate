#include "pch.hpp"

#include "profiler.hpp"

#include "logging.hpp"

namespace ce {
    Profiler::Profiler(const char* funcName, const char* file, const u64 linenum) :
        functionName(funcName), filename(file), linenumber(linenum),
        startTime(std::chrono::high_resolution_clock::now())
    {
    }

    Profiler::~Profiler()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = endTime - startTime;

        // follow the same format as the other logging functions for now.
        // eventually replace this with file stuffs.
        _log(std::cout,
             terminal::magenta,
             "[PROFILER] ",
             terminal::reset,
             filename,
             ":",
             linenumber,
             "[",
             functionName,
             "] : ",
             std::chrono::duration_cast<std::chrono::microseconds>(duration).count(),
             "µs");
    }
} // namespace ce