#include <pomegranate/pomegranate.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    using PrintHelloFunc = void (*)();

    void* clientObj = nullptr;
    PrintHelloFunc printHello = nullptr;

    do {
        clientObj = SDL_LoadObject(CLIENT_SO_FILE);
        POM_ASSERT(clientObj, SDL_GetError());
        printHello = (PrintHelloFunc)SDL_LoadFunction(clientObj, "printHello");
        POM_ASSERT(printHello, SDL_GetError());
        printHello();
        SDL_UnloadObject(clientObj);
        std::cin.get();
    } while (true);

    return 0;
}