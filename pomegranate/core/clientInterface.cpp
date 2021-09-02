#include "pch.hpp"

#include "clientInterface.hpp"

#include <utility>

namespace pom {

    Client::Client(std::string clientFilename) : clientSO(std::move(clientFilename))
    {
        POM_ASSERT(clientGetAppCreateInfo, "clientGetAppCreateInfo not properly loaded from client code.");
        POM_ASSERT(clientCreateState, "clientCreateState not properly loaded from client code.");
        POM_ASSERT(clientUpdate, "clientUpdate not properly loaded from client code.");
    }

    Client::~Client()
    {
        if (clientUnmount) {
            clientUnmount(gamestate);
        }
        if (clientEnd) {
            clientEnd(gamestate);
        }
    }

    void Client::begin()
    {
        gamestate = clientCreateState();
        if (clientBegin) {
            clientBegin(gamestate);
        }
        if (clientMount) {
            clientMount(gamestate);
        }
    }

    void Client::update(DeltaTime dt)
    {
        clientUpdate(gamestate, dt);
    }

    void Client::onEvent(const pom::InputEvent* ev)
    {
        if (clientOnInputEvent) {
            clientOnInputEvent(gamestate, ev);
        }
    }

    void Client::reload()
    {
        if (clientUnmount) {
            clientUnmount(gamestate);
        }

        clientSO.reload();

        POM_ASSERT(clientUpdate, "clientUpdate not properly loaded from client code.");

        if (clientMount) {
            clientMount(gamestate);
        }
    }

    void Client::setSOFile(std::string filename)
    {
        if (clientUnmount) {
            clientUnmount(gamestate);
        }
        if (clientEnd) {
            clientEnd(gamestate);
        }

        clientSO.setSOFilename(std::move(filename));

        // clientGetAppCreateInfo = clientSO.getFunction<ClientGetCreateInfoFunction>("clientGetAppCreateInfo");
        // clientCreateState = clientSO.getFunction<ClientCreateStateFunction>("clientCreateState");
        // clientBegin = clientSO.getFunction<ClientBeginFunction>("clientBegin");
        // clientEnd = clientSO.getFunction<ClientEndFunction>("clientEnd");
        // clientUpdate = clientSO.getFunction<ClientUpdateFunction>("clientUpdate");
        // clientOnInputEvent = clientSO.getFunction<ClientOnInputEventFunction>("clientOnInputEvent");
        // clientMount = clientSO.getFunction<ClientMountFunction>("clientMount");
        // clientUnmount = clientSO.getFunction<ClientUnmountFunction>("clientUnmount");

        POM_ASSERT(clientGetAppCreateInfo, "clientGetAppCreateInfo not properly loaded from client code.");
        POM_ASSERT(clientCreateState, "clientCreateState not properly loaded from client code.");
        POM_ASSERT(clientUpdate, "clientUpdate not properly loaded from client code.");

        gamestate = clientCreateState();
        if (clientBegin) {
            clientBegin(gamestate);
        }
        if (clientMount) {
            clientMount(gamestate);
        }
    }

} // namespace pom
