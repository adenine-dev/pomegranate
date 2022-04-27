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

    ImageLoadResult loadImage(std::string path, i32 desiredChannels)
    {
        ImageLoadResult res;
        res.pixels = stbi_load(path.c_str(), &res.width, &res.height, &res.channels, desiredChannels);

        return res;
    }

} // namespace pom