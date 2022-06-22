#pragma once

#include "base.hpp"

namespace pom {
    void showErrorMessage(const char *title, const char *message);

    void pollEvents();
} // namespace pom