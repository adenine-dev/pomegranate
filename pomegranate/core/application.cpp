#include "pch.hpp"

#include "application.hpp"

namespace pom {
    Application* Application::instance = nullptr;

    Application::Application() : frame(0)
    {
        instance = this;
    }

    Application::~Application() = default;

} // namespace pom