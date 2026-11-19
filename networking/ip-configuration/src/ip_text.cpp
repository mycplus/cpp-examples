#include "ip_text.h"

#include <cstdio>

namespace {

std::optional<unsigned> parse_decimal(std::string_view s, unsigned max)
{
    if (s.empty() || s.size() > 3 || (s.size() > 1 && s[0] == '0'))
        return std::nullopt;
    unsigned value = 0;
    for (char c : s) {
        if (c < '0' || c > '9')
            return std::nullopt;
        value = value * 10 + static_cast<unsigned>(c - '0');
    }
    if (value > max)
        return std::nullopt;
    return value;
}

bool parse_ipv4(std::string_view s, std::uint8_t* out)
{
    for (int part = 0; part < 4; ++part) {
        const std::size_t dot = s.find('.');
        const bool last = part == 3;
        if (last != (dot == std::string_view::npos))
            return false;                            // need exactly three dots
        const auto value = parse_decimal(s.substr(0, dot), 255);
        if (!value)
            return false;
        out[part] = static_cast<std::uint8_t>(*value);
        if (!last)
            s.remove_prefix(dot + 1);
    }
    return true;
}

std::optional<unsigned> parse_hex_group(std::string_view s)
{
    if (s.empty() || s.size() > 4)
        return std::nullopt;
    unsigned value = 0;
    for (char c : s) {
        unsigned digit;
        if (c >= '0' && c <= '9')      digit = static_cast<unsigned>(c - '0');
        else if (c >= 'a' && c <= 'f') digit = static_cast<unsigned>(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') digit = static_cast<unsigned>(c - 'A' + 10);
        else return std::nullopt;
        value = value * 16 + digit;
    }
    return value;
}

// Splits "a:b:c" into groups; an empty string gives no groups.
bool parse_groups(std::string_view s, unsigned* out, int& count)
{
    count = 0;
    if (s.empty())
        return true;
    for (;;) {
        if (count == 8)
            return false;
        const std::size_t colon = s.find(':');
        const auto group = parse_hex_group(s.substr(0, colon));
        if (!group)
            return false;
        out[count++] = *group;
        if (colon == std::string_view::npos)
            return true;
        s.remove_prefix(colon + 1);
    }
}

bool parse_ipv6(std::string_view s, std::uint8_t* out)
{
    unsigned head[8], tail[8];
    int head_count = 0, tail_count = 0;
    const std::size_t gap = s.find("::");
    if (gap == std::string_view::npos) {
        if (!parse_groups(s, head, head_count) || head_count != 8)
            return false;
    } else {
        if (s.find("::", gap + 1) != std::string_view::npos)
            return false;                            // "::" may appear only once
        if (!parse_groups(s.substr(0, gap), head, head_count)
            || !parse_groups(s.substr(gap + 2), tail, tail_count)
            || head_count + tail_count > 7)          // "::" stands for at least one group
            return false;
    }
    unsigned groups[8] = {};
    for (int i = 0; i < head_count; ++i)
        groups[i] = head[i];
    for (int i = 0; i < tail_count; ++i)
        groups[8 - tail_count + i] = tail[i];
    for (int i = 0; i < 8; ++i) {
        out[2 * i] = static_cast<std::uint8_t>(groups[i] >> 8);
        out[2 * i + 1] = static_cast<std::uint8_t>(groups[i] & 0xFF);
    }
    return true;
}

}    // namespace

std::optional<IpPrefix> parse_address(std::string_view text)
{
    IpPrefix ip;
    if (text.find(':') != std::string_view::npos) {
        ip.family = Family::IPv6;
        if (!parse_ipv6(text, ip.bytes.data()))
            return std::nullopt;
    } else if (!parse_ipv4(text, ip.bytes.data())) {
        return std::nullopt;
    }
    ip.prefix_length = ip.max_prefix();
    return ip;
}

std::optional<IpPrefix> parse_prefix(std::string_view text)
{
    const std::size_t slash = text.find('/');
    auto ip = parse_address(text.substr(0, slash));
    if (!ip || slash == std::string_view::npos)
        return ip;
    const auto length = parse_decimal(text.substr(slash + 1), static_cast<unsigned>(ip->max_prefix()));
    if (!length)
        return std::nullopt;
    ip->prefix_length = static_cast<int>(*length);
    return ip;
}

std::string address_to_string(const IpPrefix& ip)
{
    char buffer[48];
    if (ip.family == Family::IPv4) {
        std::snprintf(buffer, sizeof buffer, "%u.%u.%u.%u", ip.bytes[0], ip.bytes[1], ip.bytes[2], ip.bytes[3]);
        return buffer;
    }
    unsigned groups[8];
    for (int i = 0; i < 8; ++i)
        groups[i] = static_cast<unsigned>(ip.bytes[2 * i] << 8 | ip.bytes[2 * i + 1]);

    int best_start = -1, best_length = 1;            // a single zero group is not compressed
    for (int i = 0; i < 8;) {
        int j = i;
        while (j < 8 && groups[j] == 0)
            ++j;
        if (j - i > best_length) {
            best_start = i;
            best_length = j - i;
        }
        i = (j == i) ? i + 1 : j;
    }

    std::string out;
    for (int i = 0; i < 8; ++i) {
        if (i == best_start) {
            out += "::";
            i += best_length - 1;
            continue;
        }
        if (!out.empty() && out.back() != ':')
            out += ':';
        std::snprintf(buffer, sizeof buffer, "%x", groups[i]);
        out += buffer;
    }
    return out;
}

std::string prefix_to_string(const IpPrefix& ip)
{
    return address_to_string(ip) + "/" + std::to_string(ip.prefix_length);
}

std::string ipv4_mask(int prefix_length)
{
    const std::uint32_t mask = prefix_length == 0 ? 0 : ~std::uint32_t{0} << (32 - prefix_length);
    IpPrefix ip;
    for (int i = 0; i < 4; ++i)
        ip.bytes[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(mask >> (24 - 8 * i));
    return address_to_string(ip);
}

std::optional<int> prefix_from_ipv4_mask(std::string_view mask)
{
    const auto ip = parse_address(mask);
    if (!ip || ip->family != Family::IPv4)
        return std::nullopt;
    const std::uint32_t bits = std::uint32_t{ip->bytes[0]} << 24 | std::uint32_t{ip->bytes[1]} << 16
                             | std::uint32_t{ip->bytes[2]} << 8 | ip->bytes[3];
    int length = 0;
    while (length < 32 && (bits & (std::uint32_t{1} << (31 - length))))
        ++length;
    // Every bit after the first zero must be zero too: 255.0.255.0 is not a mask.
    const std::uint32_t expected = length == 0 ? 0 : ~std::uint32_t{0} << (32 - length);
    if (bits != expected)
        return std::nullopt;
    return length;
}

std::string mac_to_string(const std::uint8_t* bytes, std::size_t length)
{
    std::string out;
    char buffer[4];
    for (std::size_t i = 0; i < length; ++i) {
        std::snprintf(buffer, sizeof buffer, i == 0 ? "%02X" : "-%02X", bytes[i]);
        out += buffer;
    }
    return out;
}
