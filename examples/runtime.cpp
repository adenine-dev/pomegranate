#include <pomegranate/pomegranate.hpp>

const char* SO_FILES[] = {
    EXAMPLE_TRIANGLE_SO,
};

const u32 numExamples = sizeof(SO_FILES) / sizeof(const char*);

int main(int argc, char** argv)
{
    pom::init();
    u32 i = 0;

    {
        pom::Application app(argc, argv, SO_FILES[i]);

        while (app.shouldUpdate()) {
            app.update();

            if (pom::keyDown(pom::KeyHid::KEY_SPACE)) {
                i = (i + 1) % numExamples;
                app.setClientSO(SO_FILES[i]);
            }
        }
    }

    pom::shutdown();

    return 0;
}