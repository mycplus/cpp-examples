// Unit tests for tonemap.hpp.
#include "../src/tonemap.hpp"

#include <cmath>
#include <cstdio>

static int failures = 0;
static void check(bool ok, const char* what) {
    std::printf("%s: %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok) ++failures;
}
static bool close_to(double a, double b, double tol = 1e-6) { return std::fabs(a - b) < tol; }

int main() {
    using namespace tonemap;

    check(reinhard(0.0) == 0.0 && close_to(reinhard(1.0), 0.5), "Reinhard: 0 -> 0, 1 -> 0.5");
    check(reinhard(1e9) < 1.0, "Reinhard never reaches 1");
    check(close_to(reinhard_extended(16.0, 16.0), 1.0), "extended Reinhard maps white to 1");
    check(close_to(reinhard_extended(0.5, 1e9), reinhard(0.5)),
          "extended Reinhard with huge white equals plain Reinhard");
    check(aces_fitted(0.0) == 0.0 && aces_fitted(100.0) == 1.0, "ACES: 0 -> 0, large -> 1");
    check(close_to(aces_fitted(0.18), 0.18 * (2.51 * 0.18 + 0.03) / (0.18 * (2.43 * 0.18 + 0.59) + 0.14)),
          "ACES middle grey matches the formula");

    // Every curve must be monotonic: brighter in, brighter (or equal) out.
    bool monotonic = true;
    for (double x = 0.0; x < 64.0; x += 0.01) {
        const double y = x + 0.01;
        monotonic &= reinhard(x) < reinhard(y);
        monotonic &= reinhard_extended(x, 16.0) < reinhard_extended(y, 16.0);
        monotonic &= aces_fitted(x) <= aces_fitted(y);
        monotonic &= linear_to_srgb(x) <= linear_to_srgb(y);
    }
    check(monotonic, "all curves are monotonic on 0..64");

    check(close_to(expose(0.18, 1.0), 0.36) && close_to(expose(0.18, -1.0), 0.09), "exposure in stops");

    // Published sRGB reference points: linear 0.18 -> about 0.461, 0.5 -> about 0.735.
    check(std::fabs(linear_to_srgb(0.18) - 0.4613) < 1e-3, "sRGB encodes middle grey to ~0.461");
    check(std::fabs(linear_to_srgb(0.5) - 0.7354) < 1e-3, "sRGB encodes 0.5 to ~0.735");
    check(close_to(linear_to_srgb(0.001), 0.01292), "sRGB linear segment near black");
    check(to_8bit(linear_to_srgb(1.0)) == 255 && to_8bit(0.0) == 0, "8-bit range ends");

    std::printf(failures ? "%d test(s) failed\n" : "all tests passed\n", failures);
    return failures ? 1 : 0;
}
