#include "platform/terminal.hpp"
#include "pch.hpp"

// NOTE: its possible that modern windows will accept ansi escape codes, therefore this may be able to be modified.

namespace ce { namespace terminal {
    static void setTerminalAttributes(std::ostream& os, int16_t fg, int16_t bg)
    {
        static WORD defaultAttribs = 0;

        HANDLE handle;
        if (&os == &std::cout)
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
        else if (&os == &std::cerr)
            handle = GetStdHandle(STD_ERROR_HANDLE);
        else {
            // stringstream
            if (fg != -1) {
                switch (fg) {
                case 0: {
                    os << CE_ANSI_ESC_BLACK;
                } break;
                case FOREGROUND_RED: {
                    os << CE_ANSI_ESC_RED;
                } break;
                case FOREGROUND_GREEN: {
                    os << CE_ANSI_ESC_GREEN;
                } break;
                case FOREGROUND_RED | FOREGROUND_GREEN: {
                    os << CE_ANSI_ESC_YELLOW;
                } break;
                case FOREGROUND_BLUE: {
                    os << CE_ANSI_ESC_BLUE;
                } break;
                case FOREGROUND_RED | FOREGROUND_BLUE: {
                    os << CE_ANSI_ESC_MAGENTA;
                } break;
                case FOREGROUND_GREEN | FOREGROUND_BLUE: {
                    os << CE_ANSI_ESC_CYAN;
                } break;
                case FOREGROUND_INTENSITY: {
                    os << CE_ANSI_ESC_GREY;
                } break;
                case FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE: {
                    os << CE_ANSI_ESC_WHITE;
                } break;
                }
            } else if (bg == -1) {
                os << CE_ANSI_ESC_RESET;
                return;
            }

            if (bg != -1) {
                switch (bg) {
                case 0: {
                    os << CE_ANSI_ESC_ON_BLACK;
                } break;
                case BACKGROUND_RED: {
                    os << CE_ANSI_ESC_ON_RED;
                } break;
                case BACKGROUND_GREEN: {
                    os << CE_ANSI_ESC_ON_GREEN;
                } break;
                case BACKGROUND_RED | BACKGROUND_GREEN: {
                    os << CE_ANSI_ESC_ON_YELLOW;
                } break;
                case BACKGROUND_BLUE: {
                    os << CE_ANSI_ESC_ON_BLUE;
                } break;
                case BACKGROUND_RED | BACKGROUND_BLUE: {
                    os << CE_ANSI_ESC_ON_MAGENTA;
                } break;
                case BACKGROUND_GREEN | BACKGROUND_BLUE: {
                    os << CE_ANSI_ESC_ON_CYAN;
                } break;
                case BACKGROUND_INTENSITY: {
                    os << CE_ANSI_ESC_ON_GREY;
                } break;
                case BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE: {
                    os << CE_ANSI_ESC_ON_WHITE;
                } break;
                }
            }

            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (!GetConsoleScreenBufferInfo(handle, &consoleInfo))
            return;

        if (!defaultAttribs)
            defaultAttribs = consoleInfo.wAttributes;

        if (fg == -1 && bg == -1) {
            SetConsoleTextAttribute(handle, defaultAttribs);
        } else {
            if (bg != -1) {
                consoleInfo.wAttributes &= ~(consoleInfo.wAttributes & 0xf0);
                consoleInfo.wAttributes |= (WORD)bg;
            }

            if (fg != -1) {
                consoleInfo.wAttributes &= ~(consoleInfo.wAttributes & 0x0f);
                consoleInfo.wAttributes |= (WORD)fg;
            }

            SetConsoleTextAttribute(handle, consoleInfo.wAttributes);
        }
    }

    std::ostream& reset(std::ostream& os)
    {
        setTerminalAttributes(os, -1, -1);
        return os;
    }

    std::ostream& black(std::ostream& os)
    {
        setTerminalAttributes(os, 0, -1);
        return os;
    }

    std::ostream& red(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_RED, -1);
        return os;
    }

    std::ostream& green(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_GREEN, -1);
        return os;
    }
    std::ostream& yellow(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_RED | FOREGROUND_GREEN, -1);
        return os;
    }

    std::ostream& blue(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_BLUE, -1);
        return os;
    }

    std::ostream& magenta(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_RED | FOREGROUND_BLUE, -1);
        return os;
    }

    std::ostream& cyan(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_GREEN | FOREGROUND_BLUE, -1);
        return os;
    }

    std::ostream& white(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, -1);
        return os;
    }

    std::ostream& grey(std::ostream& os)
    {
        setTerminalAttributes(os, FOREGROUND_INTENSITY, -1);
        // os << CE_ANSI_ESC_GREY;
        return os;
    }

    std::ostream& onBlack(std::ostream& os)
    {
        setTerminalAttributes(os, -1, 0);
        return os;
    }

    std::ostream& onRed(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_RED);
        return os;
    }

    std::ostream& onGreen(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_GREEN);
        return os;
    }

    std::ostream& onYellow(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_RED | BACKGROUND_GREEN);
        return os;
    }
    std::ostream& onBlue(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_BLUE);
        return os;
    }

    std::ostream& onMagenta(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_RED | BACKGROUND_BLUE);
        return os;
    }

    std::ostream& onCyan(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_GREEN | BACKGROUND_BLUE);
        return os;
    }

    std::ostream& onWhite(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
        return os;
    }

    std::ostream& onGrey(std::ostream& os)
    {
        setTerminalAttributes(os, -1, BACKGROUND_INTENSITY);
        return os;
    }

}} // namespace ce::terminal