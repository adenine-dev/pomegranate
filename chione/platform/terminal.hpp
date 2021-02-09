#pragma once

#include <iosfwd>

#define CE_ANSI_ESC_RESET "\033[00m"
#define CE_ANSI_ESC_BLACK "\033[30m"
#define CE_ANSI_ESC_RED "\033[31m"
#define CE_ANSI_ESC_GREEN "\033[32m"
#define CE_ANSI_ESC_YELLOW "\033[33m"
#define CE_ANSI_ESC_BLUE "\033[34m"
#define CE_ANSI_ESC_MAGENTA "\033[35m"
#define CE_ANSI_ESC_CYAN "\033[36m"
#define CE_ANSI_ESC_WHITE "\033[37m"
#define CE_ANSI_ESC_GREY "\033[38;5;246m"

#define CE_ANSI_ESC_ON_BLACK "\033[40m"
#define CE_ANSI_ESC_ON_RED "\033[41m"
#define CE_ANSI_ESC_ON_GREEN "\033[42m"
#define CE_ANSI_ESC_ON_YELLOW "\033[43m"
#define CE_ANSI_ESC_ON_BLUE "\033[44m"
#define CE_ANSI_ESC_ON_MAGENTA "\033[45m"
#define CE_ANSI_ESC_ON_CYAN "\033[46m"
#define CE_ANSI_ESC_ON_WHITE "\033[47m"
#define CE_ANSI_ESC_ON_GREY "\033[48;5;246m"

namespace ce { namespace terminal {

    /// @addtogroup terminal_output
    /// @{

    /// Resets a styled ostream back to its original style.
    std::ostream& reset(std::ostream& os);

    /// Sets a styled ostream's color to black.
    std::ostream& black(std::ostream& os);

    /// Sets a styled ostream's color to red.
    std::ostream& red(std::ostream& os);

    /// Sets a styled ostream's color to green.
    std::ostream& green(std::ostream& os);

    /// Sets a styled ostream's color to blue.
    std::ostream& blue(std::ostream& os);

    /// Sets a styled ostream's color to yellow.
    std::ostream& yellow(std::ostream& os);

    /// Sets a styled ostream's color to cyan.
    std::ostream& cyan(std::ostream& os);

    /// Sets a styled ostream's color to magenta.
    std::ostream& magenta(std::ostream& os);

    /// Sets a styled ostream's color to white.
    std::ostream& white(std::ostream& os);

    /// Sets a styled ostream's color to grey.
    std::ostream& grey(std::ostream& os);

    /// Sets a styled ostream's background to black.
    std::ostream& onBlack(std::ostream& os);

    /// Sets a styled ostream's background to red.
    std::ostream& onRed(std::ostream& os);

    /// Sets a styled ostream's background to green.
    std::ostream& onGreen(std::ostream& os);

    /// Sets a styled ostream's background to blue.
    std::ostream& onBlue(std::ostream& os);

    /// Sets a styled ostream's background to yellow.
    std::ostream& onYellow(std::ostream& os);

    /// Sets a styled ostream's background to cyan.
    std::ostream& onCyan(std::ostream& os);

    /// Sets a styled ostream's background to magenta.
    std::ostream& onMagenta(std::ostream& os);

    /// Sets a styled ostream's background to white.
    std::ostream& onWhite(std::ostream& os);

    /// Sets a styled ostream's background to grey.
    std::ostream& onGrey(std::ostream& os);

    /// @}

}} // namespace ce::terminal