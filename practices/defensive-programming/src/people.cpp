// people: reads "name,age" lines, rejects bad records with a reason and a
// line number, and prints the valid ones. The C++ twin of people.c: it prints
// exactly the same output. Usage: people FILE
#include "defensive.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t name_max_len = 32;
constexpr std::size_t line_max_len = 80;

struct Person {
    std::string name;
    long age = 0;
};

// Returns the record, or sets why and returns nothing.
std::optional<Person> parse_person(std::string_view line, std::string_view& why)
{
    if (line.empty()) {
        why = "empty line";
        return std::nullopt;
    }
    const auto comma = line.find(',');
    if (comma == std::string_view::npos) {
        why = "missing comma";
        return std::nullopt;
    }
    if (comma == 0) {
        why = "empty name";
        return std::nullopt;
    }
    if (comma > name_max_len) {
        why = "name longer than 32 characters";
        return std::nullopt;
    }
    long age = 0;
    const auto st = defensive::parse_long(line.substr(comma + 1), 0, 150, age);
    if (st != defensive::Status::ok) {
        why = defensive::to_string(st);
        return std::nullopt;
    }
    return Person{std::string(line.substr(0, comma)), age};
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " FILE\n";
        return 2;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::perror(argv[1]);
        return 1;
    }

    std::string line;
    unsigned long number = 0;
    int accepted = 0;
    int rejected = 0;
    defensive::Status st;
    while ((st = defensive::read_line(in, line, line_max_len)) != defensive::Status::eof) {
        number++;
        std::string_view why;
        if (st == defensive::Status::too_long) {
            why = "line longer than 80 characters";
        } else if (st != defensive::Status::ok) {
            why = defensive::to_string(st);
        } else if (const auto p = parse_person(line, why)) {
            std::cout << "ok    line " << number << ": " << p->name << ", " << p->age << '\n';
            accepted++;
            continue;
        }
        std::cout << "error line " << number << ": " << why << '\n';
        rejected++;
        if (st == defensive::Status::io) {
            break;
        }
    }
    std::cout << accepted << " accepted, " << rejected << " rejected\n";
    return rejected == 0 && !in.bad() ? 0 : 1;
}
