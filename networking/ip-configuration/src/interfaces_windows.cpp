// interfaces_windows.cpp - Windows implementation with the IP Helper API:
// GetAdaptersAddresses to list, CreateUnicastIpAddressEntry and
// DeleteUnicastIpAddressEntry to add and remove addresses.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <netioapi.h>

#include "interfaces.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace {

std::string to_utf8(const wchar_t* text)
{
    const int size = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 1)
        return {};
    std::string out(static_cast<std::size_t>(size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text, -1, &out[0], size, nullptr, nullptr);
    out.pop_back();    // drop the terminating null
    return out;
}

bool to_ip_prefix(const SOCKADDR* address, int prefix_length, IpPrefix& ip)
{
    if (address->sa_family == AF_INET) {
        ip.family = Family::IPv4;
        std::memcpy(ip.bytes.data(), &reinterpret_cast<const sockaddr_in*>(address)->sin_addr, 4);
    } else if (address->sa_family == AF_INET6) {
        ip.family = Family::IPv6;
        std::memcpy(ip.bytes.data(), &reinterpret_cast<const sockaddr_in6*>(address)->sin6_addr, 16);
    } else {
        return false;
    }
    ip.prefix_length = prefix_length;
    return true;
}

// The row that identifies one address on one interface.
MIB_UNICASTIPADDRESS_ROW make_row(unsigned interface_index, const IpPrefix& ip)
{
    MIB_UNICASTIPADDRESS_ROW row;
    InitializeUnicastIpAddressEntry(&row);    // sets lifetimes, origin and flags to sane defaults
    row.InterfaceIndex = interface_index;
    row.OnLinkPrefixLength = static_cast<UINT8>(ip.prefix_length);
    if (ip.family == Family::IPv4) {
        row.Address.si_family = AF_INET;
        std::memcpy(&row.Address.Ipv4.sin_addr, ip.bytes.data(), 4);
    } else {
        row.Address.si_family = AF_INET6;
        std::memcpy(&row.Address.Ipv6.sin6_addr, ip.bytes.data(), 16);
    }
    return row;
}

}    // namespace

std::vector<Interface> list_interfaces(int& error)
{
    error = 0;
    // Microsoft recommends starting with a 15 KB buffer and retrying with the
    // size the call reports, because adapters can appear between calls.
    ULONG size = 15000;
    std::vector<unsigned char> buffer;
    ULONG result = ERROR_BUFFER_OVERFLOW;
    for (int attempt = 0; attempt < 3 && result == ERROR_BUFFER_OVERFLOW; ++attempt) {
        buffer.resize(size);
        result = GetAdaptersAddresses(AF_UNSPEC,
                                      GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                      nullptr, reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data()), &size);
    }
    if (result != ERROR_SUCCESS) {
        error = static_cast<int>(result);
        return {};
    }

    std::vector<Interface> list;
    for (auto* adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data()); adapter != nullptr;
         adapter = adapter->Next) {
        Interface in;
        in.name = to_utf8(adapter->FriendlyName);
        // IfIndex is 0 when IPv4 is disabled on the adapter; then use the IPv6 index.
        in.index = adapter->IfIndex != 0 ? adapter->IfIndex : adapter->Ipv6IfIndex;
        in.up = adapter->OperStatus == IfOperStatusUp;
        in.loopback = adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK;
        if (adapter->PhysicalAddressLength > 0)
            in.mac = mac_to_string(adapter->PhysicalAddress, adapter->PhysicalAddressLength);
        for (auto* unicast = adapter->FirstUnicastAddress; unicast != nullptr; unicast = unicast->Next) {
            IpPrefix ip;
            if (to_ip_prefix(unicast->Address.lpSockaddr, unicast->OnLinkPrefixLength, ip))
                in.addresses.push_back(ip);
        }
        list.push_back(in);
    }
    std::sort(list.begin(), list.end(), [](const Interface& a, const Interface& b) { return a.index < b.index; });
    return list;
}

int add_address(unsigned interface_index, const IpPrefix& address)
{
    // The address lasts until the next restart or until the adapter is reset:
    // CreateUnicastIpAddressEntry does not write to the persistent store.
    MIB_UNICASTIPADDRESS_ROW row = make_row(interface_index, address);
    return static_cast<int>(CreateUnicastIpAddressEntry(&row));
}

int remove_address(unsigned interface_index, const IpPrefix& address)
{
    MIB_UNICASTIPADDRESS_ROW row = make_row(interface_index, address);
    // Deleting an address that is not there fails with ERROR_INVALID_PARAMETER,
    // which hides the real cause. Look it up first: GetUnicastIpAddressEntry
    // reports a missing address as ERROR_NOT_FOUND.
    MIB_UNICASTIPADDRESS_ROW existing = row;
    const DWORD lookup = GetUnicastIpAddressEntry(&existing);
    if (lookup != NO_ERROR)
        return static_cast<int>(lookup);
    return static_cast<int>(DeleteUnicastIpAddressEntry(&row));
}

bool is_access_denied(int error) { return error == ERROR_ACCESS_DENIED; }
bool is_already_present(int error) { return error == ERROR_OBJECT_ALREADY_EXISTS; }
bool is_not_found(int error) { return error == ERROR_NOT_FOUND || error == ERROR_FILE_NOT_FOUND; }
bool is_not_supported(int error) { return error == ERROR_NOT_SUPPORTED; }

std::string error_text(int error)
{
    const char* what = nullptr;
    switch (error) {
    case ERROR_ACCESS_DENIED:         what = "access denied: run as administrator"; break;
    case ERROR_OBJECT_ALREADY_EXISTS: what = "the address is already assigned"; break;
    case ERROR_NOT_FOUND:
    case ERROR_FILE_NOT_FOUND:        what = "the address or interface was not found"; break;
    case ERROR_INVALID_PARAMETER:     what = "invalid parameter"; break;
    case ERROR_NOT_SUPPORTED:         what = "not supported: is IPv6 disabled?"; break;
    default: break;
    }
    std::string text = "error " + std::to_string(error);
    if (what)
        text += std::string(" (") + what + ")";
    return text;
}
