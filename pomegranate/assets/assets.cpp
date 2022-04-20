#include "pch.hpp"

#include "assets.hpp"

#include "platform/platform.hpp"

namespace pom {
    std::string getAssetPath()
    {
        // NOTE: this is liable to vary, most notably in release mode, when this will return `./assets` or similar.
        return "../../assets/";
    }

    std::string getCachePath()
    {
        return platform::getUserPath() + "cache/";
    }
} // namespace pom