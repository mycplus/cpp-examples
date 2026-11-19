// ipaddr.cpp - list network interfaces, and add or remove IP addresses.
//
//   ipaddr list
//   ipaddr add    <interface> <address>/<prefix length>
//   ipaddr remove <interface> <address>/<prefix length>
//
// <interface> is an interface index or name ("Ethernet", "eth0").
// Adding and removing need administrator rights (Windows) or root (Linux).
#include "interfaces.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

int usage()
{
    std::cout << "usage: ipaddr list\n"
                 "       ipaddr add    <interface> <address>/<prefix length>\n"
                 "       ipaddr remove <interface> <address>/<prefix length>\n";
    return 2;
}

void print(const Interface& in)
{
    std::cout << in.index << "  " << in.name << "  " << (in.up ? "up" : "down");
    if (in.loopback)
        std::cout << "  loopback";
    if (!in.mac.empty())
        std::cout << "  MAC " << in.mac;
    std::cout << '\n';
    for (const IpPrefix& ip : in.addresses) {
        std::cout << "    " << prefix_to_string(ip);
        if (ip.family == Family::IPv4)
            std::cout << "  (mask " << ipv4_mask(ip.prefix_length) << ")";
        std::cout << '\n';
    }
}

// Accepts an index ("12") or a name ("Ethernet"). Returns 0 if there is no match.
unsigned find_interface(const std::vector<Interface>& list, const std::string& wanted)
{
    for (const Interface& in : list)
        if (in.name == wanted || std::to_string(in.index) == wanted)
            return in.index;
    return 0;
}

}    // namespace

int main(int argc, char** argv)
{
    if (argc < 2)
        return usage();
    const std::string command = argv[1];

    int error = 0;
    const std::vector<Interface> list = list_interfaces(error);
    if (error != 0) {
        std::cout << "cannot list interfaces: " << error_text(error) << '\n';
        return 1;
    }

    if (command == "list" && argc == 2) {
        for (const Interface& in : list)
            print(in);
        return 0;
    }
    if ((command != "add" && command != "remove") || argc != 4)
        return usage();

    const auto address = parse_prefix(argv[3]);
    if (!address) {
        std::cout << "\"" << argv[3] << "\" is not a valid address such as 192.168.1.50/24 or fd00::5/64\n";
        return 1;
    }
    const unsigned index = find_interface(list, argv[2]);
    if (index == 0) {
        std::cout << "no interface named or numbered \"" << argv[2] << "\" (see: ipaddr list)\n";
        return 1;
    }

    error = command == "add" ? add_address(index, *address) : remove_address(index, *address);
    if (error != 0) {
        std::cout << command << " " << prefix_to_string(*address) << " failed: " << error_text(error) << '\n';
        return 1;
    }
    std::cout << (command == "add" ? "added " : "removed ") << prefix_to_string(*address)
              << (command == "add" ? " to " : " from ") << "interface " << index << '\n';
    return 0;
}
