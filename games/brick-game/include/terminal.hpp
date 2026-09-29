// terminal.hpp - raw keyboard input and output for Windows consoles and POSIX
// terminals. The destructor restores the terminal, however the game ends.
#ifndef BRICK_TERMINAL_HPP
#define BRICK_TERMINAL_HPP

#include <string_view>

namespace brick {

enum class Key { none, left, right, pause, quit };

class Terminal {
public:
    Terminal();                     // throws std::runtime_error if not a terminal
    ~Terminal();
    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    Key key();                      // next pending key, or Key::none; never blocks
    void write(std::string_view s);
};

}  // namespace brick

#endif
