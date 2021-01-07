#include "sycamore/sycamore.hpp"

int main()
{
    SM_LOG_INFO("hello world");

    sm::maths::vec2 v2 = { 2.0f, 3.0f };
    SM_LOG_INFO(v2);
    SM_LOG_INFO(v2.x);
    SM_LOG_INFO(v2[0]);

    sm::platform::openErrorWindow("title", "body");

    // {
    //     SM_PROFILE_FUNCTION();

    //     for (int i = 0; i < 1000; i++) {
    //         std::cout << i << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}