#pragma once

#include "base.hpp"

#include "gfx/texture.hpp"

namespace pom {
    struct IBLEnvironment {
        Ref<gfx::Texture> skybox;
        Ref<gfx::TextureView> skyboxView;

        Ref<gfx::Texture> irradianceMap;
        Ref<gfx::TextureView> irradianceMapView;

        Ref<gfx::Texture> prefilteredEnvMap;
        Ref<gfx::TextureView> prefilteredEnvMapView;
    };

    POM_API Ref<gfx::Texture> cubemapFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                         Ref<gfx::TextureView> equirectangularView);
    POM_API Ref<gfx::Texture> irradianceMapFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                               Ref<gfx::TextureView> equirectangularView);
    POM_API Ref<gfx::Texture> prefilteredEnvMapFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                                   Ref<gfx::TextureView> equirectangularView,
                                                                   u8 mips);

    POM_API IBLEnvironment createEnviornmentFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                                Ref<gfx::TextureView> equirectangularView);

    POM_API IBLEnvironment loadIBLEnvironment(std::string equirectangularMapPath, bool cache = true);

} // namespace pom