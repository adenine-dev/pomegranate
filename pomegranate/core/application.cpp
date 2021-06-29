#include "pch.hpp"

#include "application.hpp"
#include "platform/platform.hpp"

namespace pom {
    Application* Application::instance = nullptr;

    Application::Application(int argc, char** argv, std::string clientSOFile) :
        client(std::move(clientSOFile)), createInfo(client.clientGetAppCreateInfo(argc, argv)),
        mainWindow(createInfo->name, createInfo->graphicsAPI)
    {
        POM_ASSERT(!instance, "Attempting to create multiple Application");

        instance = this;

        client.begin();

        mainWindow.setEventHandler([&](const pom::InputEvent& ev) {
#ifdef _DEBUG
            // TODO: its prob good to detect this change somehow then act on it, but this is good enough for now.
            switch (ev.type) {
            case pom::InputEventType::KEY_DOWN: {
                if (ev.getKeycode() == pom::Keycode::R) {
                    client.reload();
                }
            } break;
            default: {
            }
            }
#endif
            client.onEvent(&ev);
        });
    }

    Application::~Application() = default;

    void Application::update()
    {
        pom::DeltaTime dt = timer.elapsed();

        frame++;
        timer.reset();
        pom::Window::pollEvents();

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
    }

} // namespace pom