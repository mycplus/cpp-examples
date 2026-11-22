// Must not compile with warnings as errors on GCC, Clang or MSVC: the
// [[nodiscard]] status of parse_long is ignored.
#include "../src/defensive.hpp"

int main()
{
    long port = 8080;
    defensive::parse_long("80x", 1, 65535, port);
    return static_cast<int>(port % 256);
}
