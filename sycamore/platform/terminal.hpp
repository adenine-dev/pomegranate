#pragma once

#include <iosfwd>

#define SM_ANSI_ESC_RESET "\033[00m"
#define SM_ANSI_ESC_BLACK "\033[30m"
#define SM_ANSI_ESC_RED "\033[31m"
#define SM_ANSI_ESC_GREEN "\033[32m"
#define SM_ANSI_ESC_YELLOW "\033[33m"
#define SM_ANSI_ESC_BLUE "\033[34m"
#define SM_ANSI_ESC_MAGENTA "\033[35m"
#define SM_ANSI_ESC_CYAN "\033[36m"
#define SM_ANSI_ESC_WHITE "\033[37m"
#define SM_ANSI_ESC_GREY "\033[38;5;246m"

#define SM_ANSI_ESC_ON_BLACK "\033[40m"
#define SM_ANSI_ESC_ON_RED "\033[41m"
#define SM_ANSI_ESC_ON_GREEN "\033[42m"
#define SM_ANSI_ESC_ON_YELLOW "\033[43m"
#define SM_ANSI_ESC_ON_BLUE "\033[44m"
#define SM_ANSI_ESC_ON_MAGENTA "\033[45m"
#define SM_ANSI_ESC_ON_CYAN "\033[46m"
#define SM_ANSI_ESC_ON_WHITE "\033[47m"
#define SM_ANSI_ESC_ON_GREY "\033[48;5;246m"

namespace sm { namespace terminal {
    std::ostream& reset(std::ostream& os);

    std::ostream& black(std::ostream& os);
    std::ostream& red(std::ostream& os);
    std::ostream& green(std::ostream& os);
    std::ostream& blue(std::ostream& os);
    std::ostream& yellow(std::ostream& os);
    std::ostream& cyan(std::ostream& os);
    std::ostream& magenta(std::ostream& os);
    std::ostream& white(std::ostream& os);
    std::ostream& grey(std::ostream& os);

    std::ostream& onBlack(std::ostream& os);
    std::ostream& onRed(std::ostream& os);
    std::ostream& onGreen(std::ostream& os);
    std::ostream& onBlue(std::ostream& os);
    std::ostream& onYellow(std::ostream& os);
    std::ostream& onCyan(std::ostream& os);
    std::ostream& onMagenta(std::ostream& os);
    std::ostream& onWhite(std::ostream& os);
    std::ostream& onGrey(std::ostream& os);

}} // namespace sm::terminal