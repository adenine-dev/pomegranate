#include "sycamore/sycamore.hpp"

int main()
{
    SM_LOG_INFO("hello world");

    {
        SM_PROFILE_FUNCTION();

        for (int i = 0; i < 1000; i++) {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}