#pragma once

#include "base.hpp"

namespace pom {
    /// Shows a debug error message window with the specified title and message.
    void showErrorMessage(const char *title, const char *message);

    /// Polls for window events, should be called once per update.
    void pollEvents();
} // namespace pom