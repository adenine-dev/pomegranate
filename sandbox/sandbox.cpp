#include <pomegranate/pomegranate.hpp>

struct GameState {
    i32 i = 4;
};

POM_EXPORT pom::AppCreateInfo clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    return { .name = "Pomegranate Sandbox Application" };
}

POM_EXPORT GameState* clientCreateState()
{
    return new GameState();
}

POM_EXPORT void clientBegin(GameState* gameState)
{
    POM_LOG_INFO("begin, ", gameState->i);
    gameState->i = 2;
}

POM_EXPORT void clientMount(GameState* gameState)
{
    POM_LOG_INFO("mount, ", gameState->i);
    gameState->i = 5;
}

POM_EXPORT void clientUpdate(GameState* gameState, pom::DeltaTime dt)
{
    gameState->i++;
    POM_LOG_INFO("dt:", dt, "ms");
}

POM_EXPORT void clientUnmount(GameState* gameState)
{
    gameState->i = 1;
    POM_LOG_INFO("unmount");
}

POM_EXPORT void clientEnd(GameState* gameState)
{
    delete gameState;
    POM_LOG_INFO("end");
}
