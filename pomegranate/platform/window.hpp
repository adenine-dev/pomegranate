#pragma once

#include <functional>

#include <SDL.h>

#include "debug/logging.hpp"
#include "graphics/gfx/context.hpp"
#include "graphics/gfx/gfx.hpp"
#include "inputEvent.hpp"
#include "maths/maths.hpp"

namespace pom {

    /// @ingroup platform
    /// @brief Represents either a window (on desktop platforms) or surface (on mobile platforms)
    class POM_API Window {
    public:
        /// Event handler function pointer, typedefed for convenience. Passed to setEventHandler
        using EventHandler = std::function<void(const InputEvent&)>;

        /// @param title: Initial title (the text that goes in the titlebar, if present) of the
        /// window.
        /// @param position: Initial position of the window, if it is negative then it will be left
        /// to the operating system to decide where it is located.
        /// @param enableVSync: `true` if the window should use VSync.
        /// @param size: Initial size of the window, if it is negative then it will default to
        /// 720x480, this behavior is subject to change.
        /// @param api: The backend graphics API for this window.
        explicit Window(const char* title,
                        gfx::GraphicsAPI api,
                        bool enableVSync,
                        const maths::ivec2& position = { -1, -1 },
                        const maths::ivec2& size = { -1, -1 });
        ~Window();

        /// Returns true if the operating system or user says that this window should close, this
        /// should be checked each update and obeyed. The exception being if a window is tied to
        /// some state that the user may not want to lose.
        [[nodiscard]] inline bool shouldClose() const
        {
            return closeRequested;
        }

        /// Returns true if VSync is enabled.
        [[nodiscard]] inline bool isVsync() const
        {
            return enableVSync;
        }

        /// Returns the graphics context associated with this window.
        [[nodiscard]] inline gfx::Context* getContext() const
        {
            return graphicsContext;
        }

        /// Returns true if the window is currently minimized.
        [[nodiscard]] inline bool isMinimized() const
        {
            return minimized;
        }

        // tbh there shouldn't be a reason to include this.
        // /// Returns the SDL window pointer, should be used if more direct access to the window is
        // /// needed. Primarily meant to be used internally.
        // /// @warning Messing with this without knowing what you are doing can an will cause crashes,
        // /// for example calling `SDL_SetWindowData` will likely break things.
        // [[nodiscard]] inline SDL_Window* getSDLHandle()
        // {
        //     return windowHandle;
        // }

        /// Returns the vulkan surface from SDL. Only applicable when vulkan is enabled and is the
        /// chosen GraphicsAPI for this window, may fail otherwise. Should be freed with `vkDestroySurfaceKHR`.
        [[nodiscard]] VkSurfaceKHR getVulkanSurface(VkInstance instance) const;

        /// Returns the vulkan drawable extent for swapchains. Only applicable when vulkan is enabled and is the chosen
        /// GraphicsAPI for this window, may fail otherwise.
        [[nodiscard]] VkExtent2D getVulkanDrawableExtent() const;

        /// Sets the function that will be called whenever this window receives an InputEvent. Will
        /// assert if invalid.
        inline void setEventHandler(EventHandler fn)
        {
            POM_ASSERT(fn, "Attempting to pass an invalid function pointer to `Window::setEventHandler`");
            callbackFn = std::move(fn);
        }

        /// Polls the platform for events that have happened in the past update, should be called
        /// once every update. Events are handled through their source Window's event callback.
        static void pollEvents();

    private:
        POM_NOCOPY(Window);

        SDL_Window* windowHandle = nullptr;
        bool closeRequested = false;
        bool minimized = false;
        bool enableVSync;

        gfx::Context* graphicsContext;

        // NOTE: this starts empty so if a client wants to do simulation stuff, they can without
        // setting up an event handler.
        EventHandler callbackFn = [](const InputEvent& /*event*/) {};

        static constexpr const char* POM_SDL_WINDOW_PTR = "POM_WIN";
    };
} // namespace pom