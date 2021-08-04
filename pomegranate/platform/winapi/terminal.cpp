#include "pch.hpp"

#include "platform/terminal.hpp"

#include "windows.h"

// NOTE: its possible that modern windows will accept ansi escape codes, therefore this may be able
// to be modified.

namespace pom::terminal {
    bool init_terminal()
    {
        // prepare std output and std error streams to allow virtual terminal sequences.
        // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (handle == INVALID_HANDLE_VALUE) { // NOLINT: this is the recommended way to do it ¯\_(ツ)_/¯
            return false;
        }

        DWORD dwMode = 0;
        if (!GetConsoleMode(handle, &dwMode)) {
            return false;
        }

        if (!SetConsoleMode(handle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            return false;
        }

        handle = GetStdHandle(STD_ERROR_HANDLE);
        if (handle == INVALID_HANDLE_VALUE) { // NOLINT
            return false;
        }

        if (!GetConsoleMode(handle, &dwMode)) {
            return false;
        }

        if (!SetConsoleMode(handle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            return false;
        }

        return true;
    }

    std::ostream& reset(std::ostream& os)
    {
        os << POM_ANSI_ESC_RESET;
        return os;
    }

    std::ostream& black(std::ostream& os)
    {
        os << POM_ANSI_ESC_BLACK;
        return os;
    }

    std::ostream& red(std::ostream& os)
    {
        os << POM_ANSI_ESC_RED;
        return os;
    }

    std::ostream& green(std::ostream& os)
    {
        os << POM_ANSI_ESC_GREEN;
        return os;
    }
    std::ostream& yellow(std::ostream& os)
    {
        os << POM_ANSI_ESC_YELLOW;
        return os;
    }

    std::ostream& blue(std::ostream& os)
    {
        os << POM_ANSI_ESC_BLUE;
        return os;
    }

    std::ostream& magenta(std::ostream& os)
    {
        os << POM_ANSI_ESC_MAGENTA;
        return os;
    }

    std::ostream& cyan(std::ostream& os)
    {
        os << POM_ANSI_ESC_CYAN;
        return os;
    }

    std::ostream& white(std::ostream& os)
    {
        os << POM_ANSI_ESC_WHITE;
        return os;
    }

    std::ostream& grey(std::ostream& os)
    {
        os << POM_ANSI_ESC_GREY;
        return os;
    }

    std::ostream& onBlack(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_BLACK;
        return os;
    }

    std::ostream& onRed(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_RED;
        return os;
    }

    std::ostream& onGreen(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_GREEN;
        return os;
    }

    std::ostream& onYellow(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_YELLOW;
        return os;
    }
    std::ostream& onBlue(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_BLUE;
        return os;
    }

    std::ostream& onMagenta(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_MAGENTA;
        return os;
    }

    std::ostream& onCyan(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_CYAN;
        return os;
    }

    std::ostream& onWhite(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_WHITE;
        return os;
    }

    std::ostream& onGrey(std::ostream& os)
    {
        os << POM_ANSI_ESC_ON_GREY;
        return os;
    }

} // namespace pom::terminal