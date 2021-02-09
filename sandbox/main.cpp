#include "chione/chione.hpp"

int main(int argc, char* argv[])
{
    CE_LOG_INFO(argc, argv);
    // CE_LOG_INFO("hello world");

    ce::maths::vec2 v2 = { 2.0f, 3.0f };
    CE_LOG_TRACE(v2[1]);
    CE_LOG_INFO(v2);
    CE_LOG_DEBUG(v2);
    CE_LOG_WARN(v2.x);
    CE_LOG_ERROR(v2[0]);

    {
        CE_PROFILE_SCOPE("Printing colors");

#define COLOR(c, oc) std::cout << c << #c << reset << " " << oc << #oc << reset << std::endl;
        using namespace ce::terminal;
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

    ce::platform::openErrorWindow("title", "body");

    CE_ASSERT(true, "noprint");
    CE_ASSERT(false, "hi");
    CE_ASSERT(false, "more hi");

    // {
    //     CE_PROFILE_FUNCTION();

    //     for (int i = 0; i < 1000; i++) {
    //         std::cout << i << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}