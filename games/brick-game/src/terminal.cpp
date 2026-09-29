// terminal.cpp - everything platform-specific lives in this file.
#include "terminal.hpp"

#include <cstdio>
#include <stdexcept>

#if defined(_WIN32)
#include <conio.h>
#include <windows.h>
#else
#include <csignal>
#include <termios.h>
#include <unistd.h>
#endif

namespace brick {

namespace {
#if defined(_WIN32)
HANDLE out_handle;
DWORD saved_mode;
volatile LONG interrupted = 0;

BOOL WINAPI on_ctrl(DWORD)
{
    InterlockedExchange(&interrupted, 1);       // the game loop sees Key::quit
    return TRUE;
}
#else
termios saved;
struct sigaction saved_int, saved_term;        // handlers to put back
volatile std::sig_atomic_t interrupted = 0;

extern "C" void on_signal(int)
{
    interrupted = 1;                            // the game loop sees Key::quit
}

int read_byte()
{
    unsigned char c;
    return ::read(STDIN_FILENO, &c, 1) == 1 ? c : -1;
}
#endif
}  // namespace

Terminal::Terminal()
{
#if defined(_WIN32)
    out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (out_handle == INVALID_HANDLE_VALUE || !GetConsoleMode(out_handle, &saved_mode) ||
        !SetConsoleMode(out_handle, saved_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        throw std::runtime_error("needs an interactive terminal");
    SetConsoleCtrlHandler(on_ctrl, TRUE);
#else
    if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &saved) != 0)
        throw std::runtime_error("needs an interactive terminal");
    termios raw = saved;
    raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
        throw std::runtime_error("cannot switch the terminal to raw mode");
    struct sigaction sa {};
    sa.sa_handler = on_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, &saved_int);
    sigaction(SIGTERM, &sa, &saved_term);
#endif
    write("\x1b[?25l");                         // hide the cursor
}

Terminal::~Terminal()
{
    write("\x1b[?25h\x1b[0m\r\n");
#if defined(_WIN32)
    SetConsoleCtrlHandler(on_ctrl, FALSE);
    SetConsoleMode(out_handle, saved_mode);
#else
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
    sigaction(SIGINT, &saved_int, nullptr);
    sigaction(SIGTERM, &saved_term, nullptr);
#endif
}

Key Terminal::key()
{
#if defined(_WIN32)
    if (InterlockedExchange(&interrupted, 0))
        return Key::quit;
    while (_kbhit()) {
        const int c = _getch();
        if (c == 0 || c == 224) {               // extended key prefix
            const int code = _getch();
            if (code == 75) return Key::left;
            if (code == 77) return Key::right;
            continue;
        }
        if (c == 'a' || c == 'A') return Key::left;
        if (c == 'd' || c == 'D') return Key::right;
        if (c == 'p' || c == 'P' || c == ' ') return Key::pause;
        if (c == 'q' || c == 'Q' || c == 27) return Key::quit;
    }
#else
    if (interrupted) {
        interrupted = 0;
        return Key::quit;
    }
    for (int c; (c = read_byte()) != -1;) {
        if (c == 27) {                          // ESC [ C / ESC [ D
            const int c2 = read_byte();
            if (c2 == -1)
                return Key::quit;               // a lone Esc
            if (c2 == '[') {
                const int c3 = read_byte();
                if (c3 == 'D') return Key::left;
                if (c3 == 'C') return Key::right;
            }
            continue;
        }
        if (c == 'a' || c == 'A') return Key::left;
        if (c == 'd' || c == 'D') return Key::right;
        if (c == 'p' || c == 'P' || c == ' ') return Key::pause;
        if (c == 'q' || c == 'Q') return Key::quit;
    }
#endif
    return Key::none;
}

void Terminal::write(std::string_view s)
{
    std::fwrite(s.data(), 1, s.size(), stdout);
    std::fflush(stdout);
}

}  // namespace brick
