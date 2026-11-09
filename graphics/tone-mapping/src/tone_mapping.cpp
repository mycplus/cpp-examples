// Prints how three tone mapping curves treat the same HDR values.
#include "tonemap.hpp"

#include <cstdio>

int main() {
    using namespace tonemap;
    // Scene-linear luminance: 0.18 is "middle grey"; 16 is a bright highlight.
    const double values[] = {0.0, 0.05, 0.18, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0};
    const double white = 16.0;

    std::printf("%8s | %8s %8s %8s %8s | %s\n", "HDR in", "clamp", "Reinhard", "ext.Rh", "ACES",
                "8-bit sRGB (ACES)");
    for (double v : values) {
        const double clipped = std::min(v, 1.0);
        const double aces = aces_fitted(v);
        std::printf("%8.2f | %8.3f %8.3f %8.3f %8.3f | %d\n", v, clipped, reinhard(v),
                    reinhard_extended(v, white), aces, to_8bit(linear_to_srgb(aces)));
    }

    // One stop of exposure doubles the input before the curve.
    std::printf("middle grey at +1 stop, ACES: %.3f\n", aces_fitted(expose(0.18, 1.0)));
    return 0;
}
