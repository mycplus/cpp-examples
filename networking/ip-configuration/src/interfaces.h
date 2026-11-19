// interfaces.h - list network interfaces and add or remove IP addresses.
// One interface, two implementations: interfaces_windows.cpp (IP Helper API)
// and interfaces_linux.cpp (getifaddrs and rtnetlink).
#pragma once

#include "ip_text.h"

#include <string>
#include <vector>

struct Interface {
    std::string name;                 // "Ethernet" on Windows, "eth0" on Linux
    unsigned index = 0;               // the number the OS uses to identify it
    std::string mac;                  // empty when the interface has none
    bool up = false;
    bool loopback = false;
    std::vector<IpPrefix> addresses;  // every unicast address, with its prefix length
};

// Every interface on this computer. On failure, returns an empty list and
// sets error to an OS error code.
std::vector<Interface> list_interfaces(int& error);

// Adds or removes one address. Both return 0 on success, otherwise an OS
// error code (a Windows error code, or an errno value on Linux).
// Both need administrator rights (Windows) or CAP_NET_ADMIN (Linux).
int add_address(unsigned interface_index, const IpPrefix& address);
int remove_address(unsigned interface_index, const IpPrefix& address);

// The meaning of the codes that callers need to tell apart.
bool is_access_denied(int error);
bool is_already_present(int error);
bool is_not_found(int error);
bool is_not_supported(int error);   // e.g. IPv6 is disabled on this system

// "error 5 (access denied)": the number, plus a short English description
// for the common cases. Not localized, so test output stays the same everywhere.
std::string error_text(int error);
