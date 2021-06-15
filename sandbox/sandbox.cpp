#include <pomegranate/pomegranate.hpp>

POM_EXPORT void clientBegin()
{
    POM_LOG_INFO("begin");
}

POM_EXPORT void clientMount()
{
    POM_LOG_INFO("mount");
}

POM_EXPORT void clientUpdate(f32 dt)
{
    static i32 i = 0;
    POM_LOG_INFO("hi ", i++);
}

POM_EXPORT void clientUnmount()
{
    POM_LOG_INFO("unmount");
}

POM_EXPORT void clientEnd()
{
    POM_LOG_INFO("end");
}
