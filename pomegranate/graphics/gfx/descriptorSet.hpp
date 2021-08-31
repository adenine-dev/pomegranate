#pragma once

#pragma once

#include "base.hpp"

#include "gfx.hpp"

namespace pom::gfx {

    class PipelineLayout;
    class Buffer;
    class Texture;

    /// @addtogroup gfx
    /// @{

    class POM_API DescriptorSet {
    public:
        /// Returns the GraphicsAPI associated with this descriptor set.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static DescriptorSet* create(PipelineLayout* layout, u32 set);

        virtual void setBuffer(u32 binding, Buffer* buffer, u32 offset = 0, u32 size = 0) = 0;
        virtual void setTexture(u32 binding, Texture* texture) = 0;

        virtual ~DescriptorSet() = default;

    protected:
        // POM_NOCOPY(DescriptorSet);
    };

    ///@}

} // namespace pom::gfx