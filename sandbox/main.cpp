#include "chione/chione.hpp"

int main(int argc, char* argv[])
{
    CE_LOG_INFO(argc, argv);

    ce::Window window("Chione Test Window");

    window.setEventHandler([](const ce::InputEvent& fn) {
        // if (fn.type == ce::InputEventType::WINDOW_MOVE) {
        //     CE_LOG_INFO(fn.getPosition());
        // } else if (fn.type == ce::InputEventType::WINDOW_RESIZE) {
        //     CE_LOG_INFO(fn.getPosition()); // should assert.
        // } else
        fn.debugPrint();
    });

    while (!window.shouldClose()) {
        ce::Window::pollEvents();
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