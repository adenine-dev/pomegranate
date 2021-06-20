#include <pomegranate/pomegranate.hpp>

struct GameState {
    i32 i = 4;
};

POM_EXPORT pom::AppCreateInfo clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    return { .name = "Pomegranate Sandbox Application", .limitUpdateRate = true, .targetUPS = 60 };
}

POM_EXPORT GameState* clientCreateState()
{
    return new GameState();
}

POM_EXPORT void clientBegin(GameState* gameState)
{
    POM_LOG_INFO("begin, ", gameState->i);
}

POM_EXPORT void clientMount(GameState* gameState)
{
    POM_LOG_INFO("mount, ", gameState->i);
}

POM_EXPORT void clientUpdate(GameState* gameState, pom::DeltaTime dt)
{
    POM_LOG_INFO("dt:", dt, "ms");
}

POM_EXPORT void clientUnmount(GameState* gameState)
{
    POM_LOG_INFO("unmount");
}

POM_EXPORT void clientEnd(GameState* gameState)
{
    delete gameState;
    POM_LOG_INFO("end");
}
