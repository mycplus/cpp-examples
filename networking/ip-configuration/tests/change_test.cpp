// change_test.cpp - adds two test addresses to the loopback interface,
// checks that they appear, removes them and checks that they are gone.
//
// The addresses come from ranges reserved for testing (198.18.0.0/15 for
// benchmarking, fd00::/8 for local use), so they cannot clash with a real
// network. Without administrator rights (Windows) or CAP_NET_ADMIN (Linux)
// the test reports itself as skipped (exit code 77).
#include "../src/interfaces.h"
#include "check.h"

#include <string>

namespace {

unsigned loopback_index()
{
    int error = 0;
    for (const Interface& in : list_interfaces(error))
        if (in.loopback)
            return in.index;
    return 0;
}

bool is_listed(unsigned index, const IpPrefix& wanted)
{
    int error = 0;
    for (const Interface& in : list_interfaces(error))
        if (in.index == index)
            for (const IpPrefix& ip : in.addresses)
                if (address_to_string(ip) == address_to_string(wanted))
                    return true;
    return false;
}

}    // namespace

int main()
{
    const unsigned lo = loopback_index();
    CHECK(lo != 0);
    if (lo == 0)
        return 1;

    for (const char* text : {"198.18.77.77/32", "fd00:6d79:6370:6c75::77/128"}) {
        const IpPrefix ip = *parse_prefix(text);
        const int added = add_address(lo, ip);
        if (is_access_denied(added)) {
            std::cout << "skipped: adding an address needs administrator rights or CAP_NET_ADMIN\n";
            return 77;
        }
        if (is_not_supported(added)) {
            std::cout << "add " << text << ": skipped, this address family is disabled here\n";
            continue;
        }
        std::cout << "add " << text << ": " << (added == 0 ? "ok" : error_text(added)) << '\n';
        CHECK_EQ(added, 0);
        CHECK(is_listed(lo, ip));

        const int again = add_address(lo, ip);
        CHECK(is_already_present(again));

        const int removed = remove_address(lo, ip);
        std::cout << "remove " << text << ": " << (removed == 0 ? "ok" : error_text(removed)) << '\n';
        CHECK_EQ(removed, 0);
        CHECK(!is_listed(lo, ip));

        const int missing = remove_address(lo, ip);
        if (!is_not_found(missing))
            std::cout << "removing it twice gave " << error_text(missing) << '\n';
        CHECK(is_not_found(missing));
    }

    std::cout << (g_failures == 0 ? "all checks passed\n" : "some checks failed\n");
    return g_failures;
}
