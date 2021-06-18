#include <pomegranate/pomegranate.hpp>

int main(int argc, char** argv)
{
    pom::platform::SharedObject client(CLIENT_SO_FILE);
    auto clientGetAppCreateInfo = client.getFunction<pom::ClientGetCreateInfoFunction>("clientGetAppCreateInfo");
    auto clientCreateState = client.getFunction<pom::ClientCreateStateFunction>("clientCreateState");
    auto clientBegin = client.getFunction<pom::ClientBeginFunction>("clientBegin");
    auto clientEnd = client.getFunction<pom::ClientEndFunction>("clientEnd");
    auto clientUpdate = client.getFunction<pom::ClientUpdateFunction>("clientUpdate");
    auto clientMount = client.getFunction<pom::ClientMountFunction>("clientMount");
    auto clientUnmount = client.getFunction<pom::ClientUnmountFunction>("clientUnmount");

    POM_ASSERT(clientGetAppCreateInfo.valid(), "clientGetAppCreateInfo not properly loaded from client code.");
    POM_ASSERT(clientCreateState.valid(), "clientCreateState not properly loaded from client code.");
    POM_ASSERT(clientBegin.valid(), "clientBegin not properly loaded from client code.");
    POM_ASSERT(clientEnd.valid(), "clientEnd not properly loaded from client code.");
    POM_ASSERT(clientUpdate.valid(), "clientUpdate not properly loaded from client code.");

    pom::AppCreateInfo appInfo = clientGetAppCreateInfo(argc, argv);

    pom::Window mainWindow(appInfo.name);

    mainWindow.setEventHandler([&](const pom::InputEvent& ev) {
        // TODO: its prob good to detect this change somehow then act on it, but this is good enough for now.
        switch (ev.type) {
        case pom::InputEventType::WINDOW_FOCUS:
        case pom::InputEventType::KEY_DOWN: {
            if (ev.type == pom::InputEventType::WINDOW_FOCUS || ev.getKeycode() == pom::Keycode::R) {
                POM_LOG_DEBUG("reloading");
                client.reload();
                POM_ASSERT(clientEnd.valid(), "clientEnd not properly loaded from client code.");
                POM_ASSERT(clientUpdate.valid(), "clientUpdate not properly loaded from client code.");
            }
        } break;
        default: {
        }
        }
    });

    pom::GameState* gameState = clientCreateState();

    clientBegin(gameState);

    if (clientMount.valid()) {
        clientMount(gameState);
    }

    pom::Timer timer;

    while (!mainWindow.shouldClose()) {
        const pom::DeltaTime dt = timer.elapsed();
        timer.reset();
        pom::Window::pollEvents();
        clientUpdate(gameState, dt);
    }

    if (clientUnmount.valid()) {
        clientUnmount(gameState);
    }

    clientEnd(gameState);

    return 0;
}