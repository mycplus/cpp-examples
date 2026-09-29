// screen.hpp - turn a new text frame into the terminal output that draws it.
#ifndef BRICK_SCREEN_HPP
#define BRICK_SCREEN_HPP

#include <string>
#include <string_view>

namespace brick {

// Clears the screen and draws the frame in full.
std::string full_redraw(std::string_view frame);

// Only the characters that differ between two frames with the same line
// lengths, each run preceded by an ANSI cursor-position sequence.
std::string diff_redraw(std::string_view prev, std::string_view cur);

}  // namespace brick

#endif
