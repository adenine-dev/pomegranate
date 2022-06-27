#include "pomegranate.hpp"

#include "debug/logging.hpp"
#include "platform/window.hpp"

class X {
public:
    X() { pom::debug("constructed"); }

    ~X() { pom::debug("destructed"); }

    u32 data;
};

int main() {
    {
        pom::Slotmap<X> sm;
        auto k = sm.emplace();
        sm.emplace();
        auto k1 = sm.emplace();
        sm.remove(k1);
        pom::debug("---");
    }
    return 0;

    pom::Window window;

    pom::debug("{}", pom::Vector<u32, 3>(1, 2, 3));

    while (!window.shouldClose()) {
        pom::pollEvents();
    }

    return 0;
}