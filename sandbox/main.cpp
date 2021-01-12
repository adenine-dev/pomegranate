#include "sycamore/sycamore.hpp"

int main()
{
    SM_LOG_INFO("hello world");

    sm::maths::vec2 v2 = { 2.0f, 3.0f };
    SM_LOG_TRACE(v2[1]);
    SM_LOG_INFO(v2);
    SM_LOG_DEBUG(v2);
    SM_LOG_WARN(v2.x);
    SM_LOG_ERROR(v2[0]);

    {
        SM_PROFILE_SCOPE("Printing colors");

#define COLOR(c, oc) std::cout << c << #c << reset << " " << oc << #oc << reset << std::endl;
        using namespace sm::terminal;
        COLOR(black, onBlack)
        COLOR(red, onRed)
        COLOR(green, onGreen)
        COLOR(yellow, onYellow)
        COLOR(blue, onBlue)
        COLOR(magenta, onMagenta)
        COLOR(cyan, onCyan)
        COLOR(grey, onGrey)
        COLOR(white, onWhite)
    }

    sm::platform::openErrorWindow("title", "body");

    SM_ASSERT(true, "noprint");
    SM_ASSERT(false, "hi");
    SM_ASSERT(false, "more hi");

    // {
    //     SM_PROFILE_FUNCTION();

    //     for (int i = 0; i < 1000; i++) {
    //         std::cout << i << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}