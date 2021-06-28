#include "pch.hpp"

#include "clientInterface.hpp"

#include <utility>

namespace pom {

    Client::Client(std::string clientFilename) : clientSO(std::move(clientFilename))
    {
        POM_ASSERT(clientGetAppCreateInfo, "clientGetAppCreateInfo not properly loaded from client code.");
        POM_ASSERT(clientCreateState, "clientCreateState not properly loaded from client code.");
        POM_ASSERT(clientBegin, "clientBegin not properly loaded from client code.");
        POM_ASSERT(clientEnd, "clientEnd not properly loaded from client code.");
        POM_ASSERT(clientUpdate, "clientUpdate not properly loaded from client code.");
        POM_ASSERT(clientOnInputEvent, "clientOnInputEvent not properly loaded from client code.");
    }

    Client::~Client()
    {
        if (clientUnmount) {
            clientUnmount(gamestate);
        }

        clientEnd(gamestate);
    }

    void Client::begin()
    {
        gamestate = clientCreateState();

        clientBegin(gamestate);
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

        POM_ASSERT(clientEnd, "clientEnd not properly loaded from client code.");
        POM_ASSERT(clientUpdate, "clientUpdate not properly loaded from client code.");
        POM_ASSERT(clientOnInputEvent, "clientOnInputEvent not properly loaded from client code.");

        if (clientMount) {
            clientMount(gamestate);
        }
    }

} // namespace pom
