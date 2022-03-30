#pragma once

#include "base.hpp"

#include <string>

namespace pom {
    /// Returns the relative path from the executable to the assets directory.
    POM_API std::string getAssetPath();
} // namespace pom