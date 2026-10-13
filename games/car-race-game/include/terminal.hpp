// terminal.hpp - raw keyboard input and output for Windows consoles and POSIX
// terminals. The destructor restores the terminal, however the game ends.
#ifndef RACE_TERMINAL_HPP
#define RACE_TERMINAL_HPP

#include <string_view>

namespace race {

enum class Key { none, left, right, nitro, pause, quit };

class Terminal {
public:
    Terminal();                     // throws std::runtime_error if not a terminal
    ~Terminal();
    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    Key key();                      // next pending key, or Key::none; never blocks
    void write(std::string_view s);
};

}  // namespace race

#endif
