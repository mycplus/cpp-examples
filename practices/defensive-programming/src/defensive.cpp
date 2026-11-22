// defensive.cpp - see defensive.hpp
#include "defensive.hpp"

#include <charconv>
#include <cstdio>
#include <fstream>
#include <istream>
#include <limits>
#include <system_error>

namespace defensive {

std::string_view to_string(Status status) noexcept
{
    switch (status) {
    case Status::ok:           return "ok";
    case Status::empty:        return "empty input";
    case Status::not_a_number: return "not a number";
    case Status::trailing:     return "extra characters after the number";
    case Status::range:        return "number out of range";
    case Status::too_long:     return "input too long";
    case Status::eof:          return "end of input";
    case Status::overflow:     return "size overflow";
    case Status::io:           return "input/output error";
    }
    return "unknown error";
}

Status parse_long(std::string_view text, long min, long max, long& out) noexcept
{
    const auto first = text.find_first_not_of(" \t\n\v\f\r");
    if (first == std::string_view::npos) {
        return Status::empty;
    }
    text.remove_prefix(first);
    if (text.size() > 1 && text[0] == '+' && text[1] != '-') {
        text.remove_prefix(1); // from_chars does not accept a '+' sign
    }
    long value = 0;
    // from_chars never throws, ignores the locale and reports where it stopped.
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error == std::errc::invalid_argument) {
        return Status::not_a_number;
    }
    if (error == std::errc::result_out_of_range || value < min || value > max) {
        return Status::range;
    }
    if (end != text.data() + text.size()) {
        return Status::trailing;
    }
    out = value;
    return Status::ok;
}

Status read_line(std::istream& in, std::string& line, std::size_t max_len)
{
    std::string text;
    if (!std::getline(in, text)) {
        return in.bad() ? Status::io : Status::eof;
    }
    if (!text.empty() && text.back() == '\r') {
        text.pop_back(); // a file with Windows line endings
    }
    if (text.size() > max_len) {
        return Status::too_long;
    }
    line = std::move(text);
    return Status::ok;
}

Status mul_size(std::size_t a, std::size_t b, std::size_t& out) noexcept
{
    if (b != 0 && a > std::numeric_limits<std::size_t>::max() / b) {
        return Status::overflow;
    }
    out = a * b;
    return Status::ok;
}

Status copy_file(const std::string& from_path, const std::string& to_path)
{
    std::ifstream in(from_path, std::ios::binary);
    if (!in) {
        return Status::io; // nothing created yet, so nothing to clean up
    }
    {
        std::ofstream out(to_path, std::ios::binary);
        if (!out) {
            return Status::io;
        }
        char buffer[4096];
        while (in.read(buffer, sizeof buffer) || in.gcount() > 0) {
            if (!out.write(buffer, in.gcount())) {
                break;
            }
        }
        const bool read_failed = in.bad();
        out.close(); // flush now, so a full disk is detected here
        if (read_failed || out.fail()) {
            std::remove(to_path.c_str()); // do not leave a half-written copy
            return Status::io;
        }
    } // the streams close themselves on every path: no goto needed
    return Status::ok;
}

} // namespace defensive
