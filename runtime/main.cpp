#include <pomegranate/pomegranate.hpp>

int main(int argc, char** argv)
{
    pom::init();

    {
        pom::Application app(argc, argv, CLIENT_SO_FILE);

        while (app.shouldUpdate()) {
            app.update();
        }
    }

    pom::shutdown();

    return 0;
}