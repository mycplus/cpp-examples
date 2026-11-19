# Change IP Address in C++

[![IP Configuration](https://github.com/mycplus/cpp-examples/actions/workflows/ip-configuration.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/ip-configuration.yml)

Companion code for [How to Change an IP Address in C++](https://www.mycplus.com/programming/cpp/change-ip-address/) on MYCPLUS: list network interfaces and add or remove IPv4 and IPv6 addresses, on Windows (IP Helper API) and Linux (getifaddrs and rtnetlink), in C++17.

| File | What it shows |
| --- | --- |
| `src/ip_text.h`, `src/ip_text.cpp` | Parsing and printing addresses and prefixes (`192.168.1.50/24`, `fd00::5/64`), masks and MAC addresses, with no OS headers |
| `src/interfaces.h` | One interface for both platforms: list interfaces, add and remove an address |
| `src/interfaces_windows.cpp` | `GetAdaptersAddresses`, `CreateUnicastIpAddressEntry`, `DeleteUnicastIpAddressEntry` |
| `src/interfaces_linux.cpp` | `getifaddrs`, and `RTM_NEWADDR` / `RTM_DELADDR` messages on a netlink socket |
| `src/ipaddr.cpp` | The command-line tool: `ipaddr list`, `ipaddr add`, `ipaddr remove` |
| `tests/` | C++ tests: the parser against `inet_pton`/`inet_ntop`, the interface list, and a real add/remove on loopback |
| `run_all.sh`, `run_all.ps1` | Every check in one command for Linux and Windows, including deliberate mistakes a test must catch |

## Build and test

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

`change_test` adds `198.18.77.77/32` and `fd00:6d79:6370:6c75::77/128` to the loopback interface, checks them and removes them. It needs administrator rights on Windows or root on Linux, and reports "skipped" otherwise. Neither address survives a restart.

## Use

```sh
ipaddr list
ipaddr add Ethernet 192.168.1.60/24      # Windows, as administrator
sudo ./ipaddr add eth0 192.168.1.60/24   # Linux
ipaddr remove Ethernet 192.168.1.60/24
```

Addresses added this way are not persistent: they disappear at the next restart (Windows also drops them when the adapter is reset). For a permanent static address, use `netsh`, `New-NetIPAddress` or your Linux distribution's network configuration.
