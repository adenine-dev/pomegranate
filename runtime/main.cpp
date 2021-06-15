#include <pomegranate/pomegranate.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    pom::platform::SharedObject client(CLIENT_SO_FILE);
    auto clientBegin = client.getFunction<pom::ClientBeginFunction>("clientBegin");
    auto clientEnd = client.getFunction<pom::ClientEndFunction>("clientEnd");
    auto clientUpdate = client.getFunction<pom::ClientUpdateFunction>("clientUpdate");
    auto clientMount = client.getFunction<pom::ClientMountFunction>("clientMount");
    auto clientUnmount = client.getFunction<pom::ClientUnmountFunction>("clientUnmount");

    POM_ASSERT(clientBegin.valid(), "clientBegin not properly loaded from client code.");
    POM_ASSERT(clientEnd.valid(), "clientEnd not properly loaded from client code.");
    POM_ASSERT(clientUpdate.valid(), "clientUpdate not properly loaded from client code.");

    clientBegin();

    if (clientMount.valid()) {
        clientMount();
    }

    while (true) {
        clientUpdate(0);
    }

    if (clientUnmount.valid()) {
        clientUnmount();
    }
    clientEnd();

    return 0;
}