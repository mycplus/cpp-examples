// list_test.cpp - list_interfaces() must find a loopback interface that
// carries 127.0.0.1/8. Every Windows and Linux system has one.
#include "../src/interfaces.h"
#include "check.h"

int main()
{
    int error = 0;
    const auto list = list_interfaces(error);
    CHECK_EQ(error, 0);
    CHECK(!list.empty());

    bool found = false;
    for (const Interface& in : list) {
        CHECK(in.index != 0);
        CHECK(!in.name.empty());
        if (!in.loopback)
            continue;
        for (const IpPrefix& ip : in.addresses)
            if (prefix_to_string(ip) == "127.0.0.1/8")
                found = true;
    }
    CHECK(found);

    std::cout << (g_failures == 0 ? "all checks passed\n" : "some checks failed\n");
    return g_failures;
}
