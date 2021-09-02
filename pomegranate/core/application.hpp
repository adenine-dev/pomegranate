#pragma once

#include "base.hpp"

#include "clientInterface.hpp"
#include "graphics/gfx/instance.hpp"
#include "platform/window.hpp"

namespace pom {
    /// @addtogroup core
    /// @{

    /// Singleton encompassing everything that exists in the application.
    class POM_API Application {
    public:
        /// Returns the current frame, this increments before each update, so for the first frame this will return 1.
        [[nodiscard]] inline u64 getFrame() const
        {
            return frame;
        }

        /// @brief Returns a reference to the main Window.
        ///
        /// Every application has a window attached to it, this is called the main window. it lasts the entire
        /// life of the application, by extension when this window is closed by the user the program will exit.
        [[nodiscard]] inline Window& getMainWindow()
        {
            return mainWindow;
        }

        // TODO: remove and change all visibilities back to normal.
        [[nodiscard]] inline gfx::Instance* getGraphicsInstance()
        {
            return graphicsInstance;
        }

        /// Returns true if the application should continue updating.
        [[nodiscard]] inline bool shouldUpdate() const
        {
            return !mainWindow.shouldClose();
        }

        /// Returns a pointer to the singleton Application instance. Extremely not threadsafe.
        [[nodiscard]] static Application* get()
        {
            return instance;
        }

    private:
        friend int ::main(int argc, char** argv);

        Application(int argc, char** argv, std::string clientSOFile);
        ~Application();

        POM_NOCOPY(Application);

        void update();
        void limitFPS(DeltaTime dt);

        void setClientSO(std::string clientSOFile);

        Client client;

        const AppCreateInfo* createInfo;

        gfx::Instance* graphicsInstance;
        Window mainWindow;

        bool paused = false;

        u64 frame = 0;
        Timer timer;

        static Application* instance;
    };

    /// @}
} // namespace pom