#pragma once

#include "base.hpp"

namespace pom {
    /// Initializes all relevant subsystems, shouldn't be called by client, called automatically in runtime.
    POM_API bool init();

    /// Destroys all subsystems, shouldn't be called by client, called automatically in runtime.
    POM_API void shutdown();
} // namespace pom