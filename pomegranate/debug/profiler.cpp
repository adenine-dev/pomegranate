#include "pch.hpp"

#include "profiler.hpp"

#include "logging.hpp"

namespace pom {
    ProfilerTimer::ProfilerTimer(const char* function, const char* file, u64 line) :
        functionName(function), fileName(file), lineNumber(line), startTime(std::chrono::high_resolution_clock::now())
    {
    }

    ProfilerTimer::~ProfilerTimer()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        duration = endTime - startTime;

        if (Profiler::active())
            Profiler::addTimer(*this);

        // TODO: replace this with file stuffs and speedscope
        // POM_TRACE("[",
        //           functionName,
        //           "] : ",
        //           std::chrono::duration_cast<std::chrono::microseconds>(duration).count(),
        //           "us");
    }

    void Profiler::begin()
    {
        if (active()) {
            POM_ERROR("Attempting to start a new profiler with one already going.");
            return;
        }

        currentProfiler = new Profiler();
        currentProfiler->output.open("result.json", std::fstream::out);
        if (currentProfiler->output.is_open()) {
            currentProfiler->output << "{"
                                       "\"otherData\": { \"application\": \"Pomegranate\" },"
                                       "\"traceEvents\": [";
            currentProfiler->output.flush();
        } else {
            POM_ERROR("Could not open ", "result.json", " for writing.");
            delete currentProfiler;
        }
    }

    void Profiler::addTimer(const ProfilerTimer& timer)
    {
        if (!active()) {
            POM_ERROR("Attempting to record to a nonexistant profiler.");
            return;
        }

        // clang-format off
        currentProfiler->output << "{"
               << R"("ph": "X",)"
               << R"("name": ")" << timer.functionName << "\","
               << R"("pid": 0,)"
               << R"("tid": 0,)" // TODO: make this not break with multithreading
               << R"("ts": )" << std::chrono::time_point_cast<std::chrono::microseconds>(timer.startTime).time_since_epoch().count() << ","
               << R"("dur": )" << timer.duration.count() 
               << "},";
        // clang-format on
    }

    void Profiler::end()
    {
        if (!active()) {
            POM_ERROR("Attempting to end a nonexistant profiler.");
            return;
        }

        currentProfiler->output << "{}]}";
        currentProfiler->output.flush();
        currentProfiler->output.close();
        delete currentProfiler;
        currentProfiler = nullptr;
    }

} // namespace pom