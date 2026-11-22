// defensive.hpp - the C++ counterparts of defensive.h.
//
// Every function returns a Status marked [[nodiscard]], and writes its
// result only on success. Buffers and files are standard library types, so
// sizes and cleanup are handled by the types themselves.
#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <string_view>

namespace defensive {

enum class Status {
    ok,
    empty,
    not_a_number,
    trailing,
    range,
    too_long,
    eof,
    overflow,
    io
};

// A short English description, the same wording as the C version.
[[nodiscard]] std::string_view to_string(Status status) noexcept;

// Parses all of text as a base-10 integer in [min, max]. Leading spaces and
// a leading '+' are allowed; anything after the number is not.
[[nodiscard]] Status parse_long(std::string_view text, long min, long max, long& out) noexcept;

// Reads one line (without "\n" or "\r\n") of at most max_len characters.
[[nodiscard]] Status read_line(std::istream& in, std::string& line, std::size_t max_len);

// out = a * b, or Status::overflow if the product does not fit in size_t.
[[nodiscard]] Status mul_size(std::size_t a, std::size_t b, std::size_t& out) noexcept;

// Copies a file. On failure no partial output file is left behind, and an
// existing file at to_path is not touched unless the copy got that far.
[[nodiscard]] Status copy_file(const std::string& from_path, const std::string& to_path);

} // namespace defensive
