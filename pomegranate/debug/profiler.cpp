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

        // TODO: replace this with file stuffs and speedscope
        POM_TRACE("[",
                  functionName,
                  "] : ",
                  std::chrono::duration_cast<std::chrono::microseconds>(duration).count(),
                  "us");
    }
} // namespace pom