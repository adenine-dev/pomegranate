#include "pch.hpp"

#include "application.hpp"

namespace pom {
    Application* Application::instance = nullptr;

    Application::Application()
    {
        instance = this;
    }

    Application::~Application()
    {
    }
} // namespace pom