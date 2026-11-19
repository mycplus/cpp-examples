// ip_text.h - IPv4 and IPv6 addresses with a prefix length, as text and as
// bytes. No operating system headers: the same code runs on Windows and Linux.
#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class Family { IPv4, IPv6 };

struct IpPrefix {
    Family family = Family::IPv4;
    std::array<std::uint8_t, 16> bytes{};   // IPv4 uses the first 4
    int prefix_length = 0;                   // 0-32 for IPv4, 0-128 for IPv6

    std::size_t size() const { return family == Family::IPv4 ? 4 : 16; }
    int max_prefix() const { return family == Family::IPv4 ? 32 : 128; }
};

// "192.168.1.50" or "fd00::1". Leading zeros in IPv4 parts are rejected,
// because some tools read "010" as octal 8.
std::optional<IpPrefix> parse_address(std::string_view text);

// "192.168.1.50/24" or "fd00::1/64". Without "/n", the prefix is the full
// length (/32 or /128), which means "this one address".
std::optional<IpPrefix> parse_prefix(std::string_view text);

// "192.168.1.50", or IPv6 in the canonical RFC 5952 form: lower case, no
// leading zeros, the longest run of two or more zero groups written as "::".
std::string address_to_string(const IpPrefix& ip);
std::string prefix_to_string(const IpPrefix& ip);   // with "/n"

// 24 -> "255.255.255.0". Defined for 0-32 only.
std::string ipv4_mask(int prefix_length);

// "255.255.255.0" -> 24. Empty for a mask whose one bits are not contiguous.
std::optional<int> prefix_from_ipv4_mask(std::string_view mask);

// "00-15-5D-01-02-03", the format ipconfig uses.
std::string mac_to_string(const std::uint8_t* bytes, std::size_t length);
