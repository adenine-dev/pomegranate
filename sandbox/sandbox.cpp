#include <pomegranate/pomegranate.hpp>

struct GameState {
    i32 i = 4;
};

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

POM_EXPORT void clientUpdate(GameState* gameState, f32 dt)
{
    gameState->i++;
    if (gameState->i % 100 == 0) {
        POM_LOG_INFO("hi ", gameState->i++);
    }
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
