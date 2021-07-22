#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "maths/vector.hpp"

namespace pom {
    class Window;

    namespace gfx {
        /// @addtogroup gfx
        /// @{

        /// A per-window interface with the low level graphics API. It is created automatically upon Window creation.
        /// Resources are shared between Contexts.
        class POM_API Context {
        public:
            /// Creates a Context with an associated Window, one window should only ever have one Context.
            static Context* create(GraphicsAPI api, Window* window);

            /// Destroys a context.
            static void destroy(Context* context);

            /// Returns the GraphicsAPI associated with this context.
            [[nodiscard]] inline virtual GraphicsAPI getAPI() const = 0;

            virtual ~Context();

            /// Recreates the swapchain at a given size, this should be called whenever the window is resized.
            virtual void recreateSwapchain(const maths::vec2& extent) = 0;

            /// Presents the rendered content to the associated Window. Should be called once per update.
            virtual void present() = 0;

        protected:
            Context(Window* window);

            Window* window;
        };

        /// @}
    } // namespace gfx
} // namespace pom