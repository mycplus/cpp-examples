// screen.cpp - redraw only the characters that changed between two frames.
#include "screen.hpp"

namespace brick {

std::string full_redraw(std::string_view frame)
{
    std::string out = "\x1b[H\x1b[2J";                 // home, clear
    for (char ch : frame) {
        if (ch == '\n')
            out += "\r\n";                              // raw mode: add CR
        else
            out += ch;
    }
    return out;
}

std::string diff_redraw(std::string_view prev, std::string_view cur)
{
    std::string out;
    std::size_t row_start_prev = 0, row_start_cur = 0;
    int row = 1;                                        // ANSI is 1-based
    while (row_start_cur < cur.size()) {
        std::size_t end_cur = cur.find('\n', row_start_cur);
        if (end_cur == std::string_view::npos)
            end_cur = cur.size();
        std::size_t end_prev = prev.find('\n', row_start_prev);
        if (end_prev == std::string_view::npos)
            end_prev = prev.size();
        const std::string_view a = row_start_prev < prev.size()
            ? prev.substr(row_start_prev, end_prev - row_start_prev) : std::string_view{};
        const std::string_view b = cur.substr(row_start_cur, end_cur - row_start_cur);

        for (std::size_t col = 0; col < b.size();) {
            if (col < a.size() && a[col] == b[col]) {
                ++col;
                continue;
            }
            const std::size_t run_start = col;          // start of a changed run
            while (col < b.size() && !(col < a.size() && a[col] == b[col]))
                ++col;
            out += "\x1b[" + std::to_string(row) + ';' + std::to_string(run_start + 1) + 'H';
            out += b.substr(run_start, col - run_start);
        }
        row_start_cur = end_cur + 1;
        row_start_prev = end_prev + 1;
        ++row;
    }
    return out;
}

}  // namespace brick
