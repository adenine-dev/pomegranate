#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "maths/vector.hpp"

namespace pom {
    class Window;

    namespace gfx {
        /// @addtogroup gfx
        /// @{

        class POM_API Context {
        public:
            static Context* create(GraphicsAPI api, Window* window);
            static void destroy(Context* context);

            [[nodiscard]] inline virtual GraphicsAPI getAPI() const = 0;

            virtual ~Context();

            virtual void recreateSwapchain(const maths::vec2& extent) = 0;

            virtual void present() = 0;

        protected:
            Context(Window* window);

            Window* window;
        };

        /// @}
    } // namespace gfx
} // namespace pom