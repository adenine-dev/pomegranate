#include "pomegranate.hpp"

#include "debug/logging.hpp"

int main()
{
    pom::hello();
    pom::trace("hello world");
    pom::debug("hello world");
    pom::info("sizeof usize {}", sizeof(usize));
    pom::warn("hello world");
    pom::error("hello world");
    pom::fatal("hello world");

    return 0;
}