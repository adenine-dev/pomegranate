#include "SDL_keycode.h"
#include "chione/chione.hpp"
#include "debug/logging.hpp"
#include "platform/input.hpp"

int main(int argc, char* argv[])
{
    CE_LOG_INFO(argc, argv);

    ce::Window window("Chione Test Window");

    window.setEventHandler([](const ce::InputEvent& ev) {
        // CE_LOG_INFO(ev.type);
        ev.debugPrint();
    });

    CE_LOG_INFO(sizeof(SDL_KeyCode));
    CE_LOG_INFO(std::is_unsigned<SDL_KeyCode>());

    while (!window.shouldClose()) {
        ce::Window::pollEvents();
        // CE_LOG_TRACE(ce::getMousePostition());
        // if (ce::mouseButtonDown(ce::MouseButton::BUTTON_1))
        //     CE_LOG_TRACE("mb1");
        // if (ce::keyDown(ce::KeyHid::KEY_A))
        //     CE_LOG_TRACE("keya");
    }

    // {
    //     CE_PROFILE_FUNCTION();

    //     for (int i = 0; i < 1000; i++) {
    //         std::cout << i << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}