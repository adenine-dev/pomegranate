#include "pomegranate.hpp"

#include "debug/logging.hpp"
#include "platform/window.hpp"

int main()
{
    pom::Window window;

    pom::debug("{}", pom::Vector<u32, 3>(1, 2, 3));

    while (!window.shouldClose()) {
        pom::pollEvents();
    }

    return 0;
}