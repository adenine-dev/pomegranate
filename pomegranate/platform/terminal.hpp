#pragma once

#include <iosfwd>

constexpr auto POM_ANSI_ESC_RESET = "\033[00m";
constexpr auto POM_ANSI_ESC_BLACK = "\033[30m";
constexpr auto POM_ANSI_ESC_RED = "\033[31m";
constexpr auto POM_ANSI_ESC_GREEN = "\033[32m";
constexpr auto POM_ANSI_ESC_YELLOW = "\033[33m";
constexpr auto POM_ANSI_ESC_BLUE = "\033[34m";
constexpr auto POM_ANSI_ESC_MAGENTA = "\033[35m";
constexpr auto POM_ANSI_ESC_CYAN = "\033[36m";
constexpr auto POM_ANSI_ESC_WHITE = "\033[37m";
constexpr auto POM_ANSI_ESC_GREY = "\033[38;5;246m";

constexpr auto POM_ANSI_ESC_ON_BLACK = "\033[40m";
constexpr auto POM_ANSI_ESC_ON_RED = "\033[41m";
constexpr auto POM_ANSI_ESC_ON_GREEN = "\033[42m";
constexpr auto POM_ANSI_ESC_ON_YELLOW = "\033[43m";
constexpr auto POM_ANSI_ESC_ON_BLUE = "\033[44m";
constexpr auto POM_ANSI_ESC_ON_MAGENTA = "\033[45m";
constexpr auto POM_ANSI_ESC_ON_CYAN = "\033[46m";
constexpr auto POM_ANSI_ESC_ON_WHITE = "\033[47m";
constexpr auto POM_ANSI_ESC_ON_GREY = "\033[48;5;246m";

namespace pom::terminal {

    /// @addtogroup terminal_output
    /// @{

    /// @anchoredname{Terminal Styles, terminal_styles}
    /// These style a styleable output to be the correct color, each color function sets the text
    /// color, and each onColor function sets the background color, `reset` resets both.
    /// @{

    POM_API std::ostream& reset(std::ostream& os);

    POM_API std::ostream& black(std::ostream& os);
    POM_API std::ostream& red(std::ostream& os);
    POM_API std::ostream& green(std::ostream& os);
    POM_API std::ostream& blue(std::ostream& os);
    POM_API std::ostream& yellow(std::ostream& os);
    POM_API std::ostream& cyan(std::ostream& os);
    POM_API std::ostream& magenta(std::ostream& os);
    POM_API std::ostream& white(std::ostream& os);
    POM_API std::ostream& grey(std::ostream& os);

    POM_API std::ostream& onBlack(std::ostream& os);
    POM_API std::ostream& onRed(std::ostream& os);
    POM_API std::ostream& onGreen(std::ostream& os);
    POM_API std::ostream& onBlue(std::ostream& os);
    POM_API std::ostream& onYellow(std::ostream& os);
    POM_API std::ostream& onCyan(std::ostream& os);
    POM_API std::ostream& onMagenta(std::ostream& os);
    POM_API std::ostream& onWhite(std::ostream& os);
    POM_API std::ostream& onGrey(std::ostream& os);

    /// @}
    /// @}

} // namespace pom::terminal