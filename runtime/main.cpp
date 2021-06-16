#include <pomegranate/pomegranate.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    pom::platform::SharedObject client(CLIENT_SO_FILE);
    auto clientCreateState = client.getFunction<pom::ClientCreateStateFunction>("clientCreateState");
    auto clientBegin = client.getFunction<pom::ClientBeginFunction>("clientBegin");
    auto clientEnd = client.getFunction<pom::ClientEndFunction>("clientEnd");
    auto clientUpdate = client.getFunction<pom::ClientUpdateFunction>("clientUpdate");
    auto clientMount = client.getFunction<pom::ClientMountFunction>("clientMount");
    auto clientUnmount = client.getFunction<pom::ClientUnmountFunction>("clientUnmount");

    POM_ASSERT(clientCreateState.valid(), "clientCreateState not properly loaded from client code.");
    POM_ASSERT(clientBegin.valid(), "clientBegin not properly loaded from client code.");
    POM_ASSERT(clientEnd.valid(), "clientEnd not properly loaded from client code.");
    POM_ASSERT(clientUpdate.valid(), "clientUpdate not properly loaded from client code.");

    pom::GameState* gameState = clientCreateState();

    clientBegin(gameState);

    if (clientMount.valid()) {
        clientMount(gameState);
    }

    i32 frame = 0;
    while (true) {
        frame++;
        clientUpdate(gameState, 0);
        if (frame % 1000 == 0) {
            POM_LOG_INFO("reloading");
            std::cin.get();
            client.reload();

            POM_ASSERT(clientCreateState.valid(), "clientCreateState not properly loaded from client code.");
            POM_ASSERT(clientBegin.valid(), "clientBegin not properly loaded from client code.");
            POM_ASSERT(clientEnd.valid(), "clientEnd not properly loaded from client code.");
            POM_ASSERT(clientUpdate.valid(), "clientUpdate not properly loaded from client code.");
        }
    }

    if (clientUnmount.valid()) {
        clientUnmount(gameState);
    }
    clientEnd(gameState);

    return 0;
}