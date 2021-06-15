#include <pomegranate/pomegranate.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    using PrintHelloFunc = void (*)();

    pom::platform::SharedObject client(CLIENT_SO_FILE);
    auto printHello = client.getFunction<PrintHelloFunc>("printHello");

    do {
        printHello();
        std::cin.get();
        client.reload();
    } while (printHello.valid());
    POM_LOG_DEBUG("printHello stopped being valid");

    return 0;
}