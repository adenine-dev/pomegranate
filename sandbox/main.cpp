#include "pomegranate.hpp"

#include "debug/logging.hpp"
#include "platform/window.hpp"

int main() {
    pom::Window window({
        .title = "test",

    });

    while (!window.shouldClose()) {
        pom::pollEvents();
    }

    return 0;
}