#include <pomegranate/pomegranate.hpp>

POM_EXPORT void printGoodbye()
{
    POM_LOG_INFO("goodbye from sandbox.");
}
POM_EXPORT void printHello()
{
    POM_LOG_INFO("hello from sandbox!");
}