// interfaces_linux.cpp - Linux implementation: getifaddrs() to list,
// an rtnetlink socket to add and remove addresses.
#include "interfaces.h"

#include <cerrno>
#include <cstring>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <memory>

namespace {

int count_prefix_bits(const unsigned char* mask, std::size_t length)
{
    int bits = 0;
    for (std::size_t i = 0; i < length; ++i)
        for (int b = 7; b >= 0; --b)
            bits += (mask[i] >> b) & 1;
    return bits;
}

Interface& find_or_add(std::vector<Interface>& list, const ifaddrs* entry)
{
    for (Interface& in : list)
        if (in.name == entry->ifa_name)
            return in;
    Interface in;
    in.name = entry->ifa_name;
    in.index = if_nametoindex(entry->ifa_name);
    in.up = (entry->ifa_flags & IFF_UP) != 0;
    in.loopback = (entry->ifa_flags & IFF_LOOPBACK) != 0;
    list.push_back(in);
    return list.back();
}

// Owns a socket file descriptor.
struct Fd {
    int fd;
    ~Fd() { if (fd >= 0) close(fd); }
};

// Sends one RTM_NEWADDR or RTM_DELADDR request and waits for the kernel's answer.
int change_address(int type, int flags, unsigned index, const IpPrefix& ip)
{
    Fd sock{socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE)};
    if (sock.fd < 0)
        return errno;

    struct Request {
        nlmsghdr header;
        ifaddrmsg message;
        char attributes[64];
    } request{};
    request.header.nlmsg_len = NLMSG_LENGTH(sizeof(ifaddrmsg));
    request.header.nlmsg_type = static_cast<unsigned short>(type);
    request.header.nlmsg_flags = static_cast<unsigned short>(NLM_F_REQUEST | NLM_F_ACK | flags);
    request.header.nlmsg_seq = 1;
    request.message.ifa_family = ip.family == Family::IPv4 ? AF_INET : AF_INET6;
    request.message.ifa_prefixlen = static_cast<unsigned char>(ip.prefix_length);
    request.message.ifa_index = index;

    // IFA_LOCAL is the interface's own address. For IPv4, IFA_ADDRESS must
    // carry the same value (it differs only on point-to-point links).
    for (unsigned short attribute : {IFA_LOCAL, IFA_ADDRESS}) {
        auto* rta = reinterpret_cast<rtattr*>(reinterpret_cast<char*>(&request) + NLMSG_ALIGN(request.header.nlmsg_len));
        rta->rta_type = attribute;
        rta->rta_len = static_cast<unsigned short>(RTA_LENGTH(ip.size()));
        std::memcpy(RTA_DATA(rta), ip.bytes.data(), ip.size());
        request.header.nlmsg_len = NLMSG_ALIGN(request.header.nlmsg_len) + RTA_ALIGN(rta->rta_len);
    }

    sockaddr_nl kernel{};
    kernel.nl_family = AF_NETLINK;
    if (sendto(sock.fd, &request, request.header.nlmsg_len, 0,
               reinterpret_cast<sockaddr*>(&kernel), sizeof kernel) < 0)
        return errno;

    // The answer is an NLMSG_ERROR message; error 0 means success.
    alignas(nlmsghdr) char reply[4096];
    const ssize_t got = recv(sock.fd, reply, sizeof reply, 0);
    if (got < 0)
        return errno;
    const auto* header = reinterpret_cast<const nlmsghdr*>(reply);
    if (got < static_cast<ssize_t>(NLMSG_LENGTH(sizeof(nlmsgerr))) || header->nlmsg_type != NLMSG_ERROR)
        return EPROTO;
    return -static_cast<const nlmsgerr*>(NLMSG_DATA(header))->error;
}

}    // namespace

std::vector<Interface> list_interfaces(int& error)
{
    error = 0;
    ifaddrs* raw = nullptr;
    if (getifaddrs(&raw) != 0) {
        error = errno;
        return {};
    }
    std::unique_ptr<ifaddrs, decltype(&freeifaddrs)> all(raw, freeifaddrs);

    std::vector<Interface> list;
    for (const ifaddrs* entry = raw; entry != nullptr; entry = entry->ifa_next) {
        Interface& in = find_or_add(list, entry);
        if (entry->ifa_addr == nullptr)
            continue;
        const int family = entry->ifa_addr->sa_family;
        if (family == AF_PACKET) {
            const auto* link = reinterpret_cast<const sockaddr_ll*>(entry->ifa_addr);
            if (link->sll_halen > 0 && !in.loopback)
                in.mac = mac_to_string(link->sll_addr, link->sll_halen);
        } else if (family == AF_INET || family == AF_INET6) {
            IpPrefix ip;
            if (family == AF_INET) {
                ip.family = Family::IPv4;
                std::memcpy(ip.bytes.data(), &reinterpret_cast<const sockaddr_in*>(entry->ifa_addr)->sin_addr, 4);
                if (entry->ifa_netmask)
                    ip.prefix_length = count_prefix_bits(reinterpret_cast<const unsigned char*>(
                        &reinterpret_cast<const sockaddr_in*>(entry->ifa_netmask)->sin_addr), 4);
            } else {
                ip.family = Family::IPv6;
                std::memcpy(ip.bytes.data(), &reinterpret_cast<const sockaddr_in6*>(entry->ifa_addr)->sin6_addr, 16);
                if (entry->ifa_netmask)
                    ip.prefix_length = count_prefix_bits(reinterpret_cast<const unsigned char*>(
                        &reinterpret_cast<const sockaddr_in6*>(entry->ifa_netmask)->sin6_addr), 16);
            }
            in.addresses.push_back(ip);
        }
    }
    std::sort(list.begin(), list.end(), [](const Interface& a, const Interface& b) { return a.index < b.index; });
    return list;
}

int add_address(unsigned interface_index, const IpPrefix& address)
{
    // NLM_F_EXCL: fail with EEXIST instead of silently replacing an existing address.
    return change_address(RTM_NEWADDR, NLM_F_CREATE | NLM_F_EXCL, interface_index, address);
}

int remove_address(unsigned interface_index, const IpPrefix& address)
{
    return change_address(RTM_DELADDR, 0, interface_index, address);
}

bool is_access_denied(int error) { return error == EPERM || error == EACCES; }
bool is_already_present(int error) { return error == EEXIST; }
bool is_not_found(int error) { return error == EADDRNOTAVAIL || error == ENODEV; }
bool is_not_supported(int error) { return error == EOPNOTSUPP || error == EAFNOSUPPORT; }

std::string error_text(int error)
{
    const char* what = nullptr;
    switch (error) {
    case EPERM:
    case EACCES:        what = "permission denied: needs root or CAP_NET_ADMIN"; break;
    case EEXIST:        what = "the address is already assigned"; break;
    case EADDRNOTAVAIL: what = "the address is not assigned to that interface"; break;
    case ENODEV:        what = "no such interface"; break;
    case EINVAL:        what = "invalid argument"; break;
    case EOPNOTSUPP:
    case EAFNOSUPPORT:  what = "not supported: is IPv6 disabled?"; break;
    default: break;
    }
    std::string text = "error " + std::to_string(error);
    if (what)
        text += std::string(" (") + what + ")";
    return text;
}
