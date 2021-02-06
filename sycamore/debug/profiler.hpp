#pragma once

#include <chrono>
#include <string>

namespace sm {

    /// @ingroup debug
    ///
    /// A scope based Profiler, not meant to be used directly, instad use the SM_PROFILE_SCOPE() and
    /// SM_PROFILE_FUNCTION() macros. Very simple, records time when it is constructed, and when it is destructed,
    /// prints the difference. Eventually this will use speedscope, [speedscope](https://github.com/jlfwong/speedscope)
    /// with this [schema](https://www.speedscope.app/file-format-schema.json)
    class Profiler {
    public:
        /// Constructor begins the timer.
        Profiler(const char* functionName, const char* filename, const uint64_t linenumber);

        /// Destructor ends the timer and logs the output.
        ~Profiler();

    private:
        const char* functionName;
        const char* filename;
        uint64_t linenumber;

        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };
} // namespace sm

/// @addtogroup debug
/// @{

/// A compiler agnostic string to identify function signatures, Modified from boost's function detection macro.
#ifndef SM_CURRENT_FUNCSIG
#    if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600))     \
        || defined(__ghs__) || defined(__DMC__) && (__DMC__ >= 0x810)
#        define SM_CURRENT_FUNCSIG __PRETTY_FUNCTION__
#    elif (defined(__FUNCSIG__) || (_MSC_VER))
#        define SM_CURRENT_FUNCSIG __FUNCSIG__
#    elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#        define SM_CURRENT_FUNCSIG __FUNCTION__
#    elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#        define SM_CURRENT_FUNCSIG __FUNC__
#    elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901) || defined(__cplusplus) && (__cplusplus >= 201103)
#        define SM_CURRENT_FUNCSIG __func__
#    elif defined(DOXYGEN)
#        define SM_CURRENT_FUNCSIG
#    else
#        warning                                                                                                       \
            "No way to detect the current function signature, define SM_CURRENT_FUNCSIG based on your compiler's way to detect it."
#        define SM_CURRENT_FUNCSIG "Unknown function signature"
#    endif
#endif

/*! \def SM_PROFILE_SCOPE(name) */
/// When put in a scope, will profile the rest of the scope (recommended to place at beginning unless you have a very
/// good reason), should be preferred over using Profiler directly. Currently only prints the output once the scope
/// ends, this behavior will change.
/// ex:
/// ```cpp
/// {
///     SM_PROFILE_SCOPE("example scope");
///     // do stuff
/// }
/// ```
#define SM_PROFILE_SCOPE(name) ::sm::Profiler _SYCAMORE_PROFILER_##__LINE__##_(name, __FILENAME__, __LINE__)

/*! \def SM_PROFILE_FUNCTION() */
/// When put in a scope, will profile the rest of the scope (recommended to place at beginning unless you have a very
/// good reason), should be preferred over using Profiler directly. Currently only prints the output once the scope
/// ends, this behavior will change.
/// ex:
/// ```cpp
/// void func() {
///     SM_PROFILE_FUNCTION();
///     // do stuff
/// }
/// ```
#define SM_PROFILE_FUNCTION() SM_PROFILE_SCOPE(SM_CURRENT_FUNCSIG)
/// @}