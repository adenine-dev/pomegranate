#include "pch.hpp"

#include "application.hpp"
#include "platform/platform.hpp"

namespace pom {
    Application* Application::instance = nullptr;

    Application::Application(int argc, char** argv, std::string clientSOFile) :
        client(std::move(clientSOFile)), createInfo(client.clientGetAppCreateInfo(argc, argv)),
        graphicsInstance(gfx::Instance::create(createInfo->name, createInfo->graphicsAPI)),
        mainWindow(createInfo->name, createInfo->graphicsAPI, createInfo->enableVsync)
    {
        POM_PROFILE_FUNCTION();

        POM_ASSERT(!instance, "Attempting to create multiple Application");

        instance = this;

        client.begin();

        mainWindow.setEventHandler([&](const pom::InputEvent& ev) {
            // POM_DEBUG(ev);
            // TODO: its prob good to detect this change somehow then act on it, but this is good enough for now.
            switch (ev.type) {
            case pom::InputEventType::KEY_DOWN: {
#ifdef POM_DEBUG
                if (ev.getKeycode() == pom::Keycode::R) {
                    POM_DEBUG("Reloading");
                    client.reload();
                }
#endif
            } break;
            case pom::InputEventType::WINDOW_RESIZE: {
                mainWindow.getContext()->recreateSwapchain({ (f32)ev.getSize().x, (f32)ev.getSize().y });
                update(); // FIXME: don't need to update only to repaint?
            } break;
            case pom::InputEventType::WINDOW_MINIMIZE: {
                paused = true;
            } break;
            case pom::InputEventType::WINDOW_FOCUS: {
                paused = false;
            } break;
            default: {
            }
            }
            client.onEvent(&ev);
        });
    }

    Application::~Application() = default;

    void Application::update()
    {
        POM_PROFILE_FUNCTION();

        pom::DeltaTime dt = timer.elapsed();
        timer.reset();

        pom::Window::pollEvents();
        if (paused) {
            return;
        }

        frame++;

        client.update(dt);

        if (createInfo->limitUpdateRate) {
            // NOTE: maybe do some analytics here?
            const f32 targetUpdateTime = 1000.f / (float)createInfo->targetUPS;
            dt = timer.elapsed();
            while (dt < targetUpdateTime) {
                platform::sleep((u32)std::max(std::floor(targetUpdateTime - dt - 1), 0.f));
                dt = timer.elapsed();
            }
        }

        graphicsInstance->update();
    }

    void Application::setClientSO(std::string clientSOFile)
    {
        POM_PROFILE_FUNCTION();

        client.setSOFile(std::move(clientSOFile));
    }

} // namespace pom