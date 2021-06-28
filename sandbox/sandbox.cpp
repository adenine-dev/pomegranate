#include <pomegranate/pomegranate.hpp>

struct GameState {
    bool printed = false;
};

POM_CLIENT_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    static const pom::AppCreateInfo aci = { .name = "Pomegranate Sandbox Application" };
    POM_LOG_INFO("clientGetAppCreateInfo");

    return &aci;
}

POM_CLIENT_EXPORT GameState* clientCreateState()
{
    auto* gc = new GameState;
    POM_LOG_INFO("clientCreateState");
    return gc;
}

POM_CLIENT_EXPORT void clientBegin(GameState* gameState)
{
    POM_LOG_INFO("clientBegin");
}

POM_CLIENT_EXPORT void clientMount(GameState* gameState)
{
    POM_LOG_INFO("clientMount");
    gameState->printed = false;
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gameState, pom::DeltaTime dt)
{
    if (!gameState->printed) {
        gameState->printed = true;
        POM_LOG_INFO("clientUpdate...", pom::Application::get()->getFrame());
    }
}

POM_CLIENT_EXPORT void clientOnInputEvent(GameState* gameState, pom::InputEvent* ev)
{
    if (ev->type == pom::InputEventType::KEY_DOWN) {
        POM_LOG_INFO("clientOnInputEvent, ", ev->getKeycode());
    }
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gameState)
{
    POM_LOG_INFO("clientUnmount");
}

POM_CLIENT_EXPORT void clientEnd(GameState* gameState)
{
    POM_LOG_INFO("clientEnd");

    delete gameState;
}
