#pragma once

#include "base.hpp"

#include <string>

namespace pom {
    /// Returns the relative path from the executable to the assets directory.
    POM_API std::string getAssetPath();

    /// Returns the absolute to the cache directory.
    POM_API std::string getCachePath();

    struct ImageLoadResult {
        void* pixels;
        i32 channels;
        i32 width;
        i32 height;
    };

    POM_API ImageLoadResult loadImage(std::string path, i32 desiredChannels);

} // namespace pom