#include <pomegranate/pomegranate.hpp>

int main(int argc, char** argv)
{
    pom::platform::init();

    pom::Application app(argc, argv, CLIENT_SO_FILE);

    while (app.shouldUpdate()) {
        app.update();
    }

    return 0;
}