// catch2_tests.cpp - unit tests with Catch2 v3. Linking Catch2::Catch2WithMain
// supplies main(); the file holds only the code under test and the tests.
#include <catch2/catch_test_macros.hpp>

#include <charconv>
#include <optional>
#include <string_view>

// Code under test: parse a whole string as a non-negative int.
std::optional<int> parse_count(std::string_view text)
{
    int value = 0;
    auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc{} || end != text.data() + text.size() || value < 0)
        return std::nullopt;
    return value;
}

TEST_CASE("valid counts parse", "[parse]")
{
    REQUIRE(parse_count("0") == 0);
    REQUIRE(parse_count("42") == 42);
    REQUIRE(parse_count("2147483647") == 2147483647);
}

TEST_CASE("invalid counts are rejected", "[parse]")
{
    SECTION("empty") { CHECK_FALSE(parse_count("").has_value()); }
    SECTION("trailing text") { CHECK_FALSE(parse_count("12abc").has_value()); }
    SECTION("negative") { CHECK_FALSE(parse_count("-5").has_value()); }
    SECTION("too large for int") { CHECK_FALSE(parse_count("2147483648").has_value()); }
}
