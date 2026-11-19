// ip_text_test.cpp - the parser and formatter, checked against fixed answers
// and against the operating system's own inet_pton and inet_ntop.
#include "../src/ip_text.h"
#include "check.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include <cstring>
#include <string>

namespace {

// What the OS makes of the same text: the canonical form, or "" if invalid.
std::string os_canonical(const std::string& text)
{
    unsigned char bytes[16];
    char out[64];
    const int family = text.find(':') != std::string::npos ? AF_INET6 : AF_INET;
    if (inet_pton(family, text.c_str(), bytes) != 1)
        return "";
    return inet_ntop(family, bytes, out, sizeof out) ? out : "";
}

std::string canonical(const std::string& text)
{
    const auto ip = parse_address(text);
    return ip ? address_to_string(*ip) : "";
}

}    // namespace

int main()
{
    // Fixed answers.
    CHECK_EQ(canonical("192.168.1.50"), "192.168.1.50");
    CHECK_EQ(canonical("0.0.0.0"), "0.0.0.0");
    CHECK_EQ(canonical("255.255.255.255"), "255.255.255.255");
    CHECK_EQ(canonical("FD00:0:0:0:0:0:0:5"), "fd00::5");
    CHECK_EQ(canonical("2001:db8:0:0:1:0:0:1"), "2001:db8::1:0:0:1");   // the first of two equal runs
    CHECK_EQ(canonical("2001:db8:0:1:1:1:1:1"), "2001:db8:0:1:1:1:1:1"); // one zero group stays
    CHECK_EQ(canonical("::"), "::");
    CHECK_EQ(canonical("::1"), "::1");
    CHECK_EQ(canonical("fe80::"), "fe80::");

    for (const char* bad : {"", "1.2.3", "1.2.3.4.5", "256.1.1.1", "1.2.3.-4", "01.2.3.4", "1..2.3",
                            "1:2:3:4:5:6:7", "1:2:3:4:5:6:7:8:9", "1::2::3", "12345::", "g::1", ":1", "1:"})
        CHECK(!parse_address(bad));

    // Prefixes.
    const auto p = parse_prefix("10.0.0.7/8");
    CHECK(p && p->prefix_length == 8 && prefix_to_string(*p) == "10.0.0.7/8");
    CHECK(parse_prefix("10.0.0.7") && parse_prefix("10.0.0.7")->prefix_length == 32);
    CHECK(parse_prefix("fd00::5/64") && parse_prefix("fd00::5/64")->prefix_length == 64);
    CHECK(!parse_prefix("10.0.0.7/33"));
    CHECK(!parse_prefix("fd00::5/129"));
    CHECK(!parse_prefix("10.0.0.7/"));
    CHECK(!parse_prefix("10.0.0.7/08"));

    // Masks.
    CHECK_EQ(ipv4_mask(0), "0.0.0.0");
    CHECK_EQ(ipv4_mask(8), "255.0.0.0");
    CHECK_EQ(ipv4_mask(20), "255.255.240.0");
    CHECK_EQ(ipv4_mask(24), "255.255.255.0");
    CHECK_EQ(ipv4_mask(32), "255.255.255.255");
    for (int n = 0; n <= 32; ++n)
        CHECK(prefix_from_ipv4_mask(ipv4_mask(n)) == n);
    CHECK(!prefix_from_ipv4_mask("255.0.255.0"));
    CHECK(!prefix_from_ipv4_mask("255.255.255.1"));

    // MAC addresses.
    const unsigned char mac[] = {0x00, 0x15, 0x5d, 0x01, 0xab, 0xff};
    CHECK_EQ(mac_to_string(mac, sizeof mac), "00-15-5D-01-AB-FF");

    // Agreement with the operating system on valid and invalid input.
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    for (const char* text : {"192.168.1.50", "10.0.0.1", "8.8.4.4", "1.2.3", "300.1.1.1", "::", "::1",
                             "fe80::1", "2001:db8::", "2001:DB8:0:0:8:800:200C:417A", "ff02::1:2",
                             "1:0:0:2:0:0:0:3", "0:0:1:0:0:0:0:0", "1::2::3", "1:2:3:4:5:6:7:8"})
        CHECK_EQ(canonical(text), os_canonical(text));

    std::cout << (g_failures == 0 ? "all checks passed\n" : "some checks failed\n");
    return g_failures;
}
