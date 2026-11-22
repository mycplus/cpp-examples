// Unit tests for defensive.cpp: the same cases as the C tests, where the
// two versions behave the same. Usage: test_defensive DATA_DIR TEMP_DIR
#include "../src/defensive.hpp"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>

using defensive::Status;

static int checks = 0;
static int failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        checks++;                                                                \
        if (!(cond)) {                                                           \
            failures++;                                                          \
            std::cout << "FAIL " << __FILE__ << ':' << __LINE__ << ": " #cond "\n"; \
        }                                                                        \
    } while (0)

static void test_parse_long()
{
    struct Case {
        const char* text;
        Status expected;
        long value;
    };
    const Case cases[] = {
        {"42", Status::ok, 42},           {"0", Status::ok, 0},
        {"100", Status::ok, 100},         {"  7", Status::ok, 7},
        {"+9", Status::ok, 9},            {"", Status::empty, 0},
        {"   ", Status::empty, 0},        {"abc", Status::not_a_number, 0},
        {"-", Status::not_a_number, 0},   {"+-5", Status::not_a_number, 0},
        {"12abc", Status::trailing, 0},   {"7 ", Status::trailing, 0},
        {"101", Status::range, 0},        {"-1", Status::range, 0},
        {"99999999999999999999", Status::range, 0},
    };
    for (const auto& c : cases) {
        long value = -12345;
        const Status st = defensive::parse_long(c.text, 0, 100, value);
        CHECK(st == c.expected);
        CHECK(value == (st == Status::ok ? c.value : -12345)); // written on success only
    }
    long v = 0;
    CHECK(defensive::parse_long(std::to_string(LONG_MIN), LONG_MIN, LONG_MAX, v) == Status::ok && v == LONG_MIN);
}

static void test_read_line()
{
    // The same lines as data/lines.txt in the C version, with a limit of 7.
    std::istringstream in("short\nthis line is too long\ncrlf\r\n\n6chars\nabcdefg\nabcdef\r\nabcdefgh\nlast");
    std::string line;
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line == "short");
    CHECK(defensive::read_line(in, line, 7) == Status::too_long && line == "short"); // unchanged
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line == "crlf");
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line.empty());
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line == "6chars");
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line == "abcdefg"); // exactly 7
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line == "abcdef");
    CHECK(defensive::read_line(in, line, 7) == Status::too_long); // 8
    CHECK(defensive::read_line(in, line, 7) == Status::ok && line == "last");
    CHECK(defensive::read_line(in, line, 7) == Status::eof);
}

static void test_sizes()
{
    const std::size_t max = std::numeric_limits<std::size_t>::max();
    std::size_t n = 0;
    CHECK(defensive::mul_size(1000, 1000, n) == Status::ok && n == 1000000);
    CHECK(defensive::mul_size(max, 1, n) == Status::ok && n == max);
    CHECK(defensive::mul_size(max / 2 + 1, 2, n) == Status::overflow);
    CHECK(defensive::mul_size(0, max, n) == Status::ok && n == 0);
}

static std::string slurp(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(f), {});
}

static void test_copy_file(const std::string& data, const std::string& tmp)
{
    const std::string from = data + "/people.txt";
    const std::string to = tmp + "/copy-of-people.txt";
    const std::string empty = tmp + "/empty.txt";
    const std::string empty_copy = tmp + "/copy-of-empty.txt";

    CHECK(defensive::copy_file(from, to) == Status::ok);
    CHECK(slurp(from) == slurp(to) && !slurp(from).empty());

    // An empty file is copied too (a plain `out << in.rdbuf()` reports failure here).
    { std::ofstream(empty, std::ios::binary); }
    CHECK(defensive::copy_file(empty, empty_copy) == Status::ok);
    CHECK(std::ifstream(empty_copy).good());

    // A missing source fails and leaves an existing target alone.
    CHECK(defensive::copy_file(data + "/no-such-file.txt", to) == Status::io);
    CHECK(slurp(to) == slurp(from));
    std::remove(to.c_str());
    std::remove(empty.c_str());
    std::remove(empty_copy.c_str());
}

static void test_to_string()
{
    CHECK(defensive::to_string(Status::range) == "number out of range");
    CHECK(defensive::to_string(static_cast<Status>(99)) == "unknown error");
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " DATA_DIR TEMP_DIR\n";
        return 2;
    }
    test_parse_long();
    test_read_line();
    test_sizes();
    test_copy_file(argv[1], argv[2]);
    test_to_string();
    std::cout << (checks - failures) << " of " << checks << " checks passed\n";
    return failures == 0 ? 0 : 1;
}
