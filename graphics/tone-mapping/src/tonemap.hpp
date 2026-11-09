// Tone mapping operators: turn HDR (unbounded) linear colour into the 0..1
// range a display can show. Companion code for the MYCPLUS article
// on graphics technologies in AAA game development with C++.
#pragma once

#include <algorithm>
#include <cmath>

namespace tonemap {

// Reinhard (2002): x / (1 + x). Never reaches 1, so nothing ever clips to white.
inline double reinhard(double x) { return x / (1.0 + x); }

// Extended Reinhard: maps `white` (the brightest value in the scene) to exactly 1.
inline double reinhard_extended(double x, double white) {
    return x * (1.0 + x / (white * white)) / (1.0 + x);
}

// Krzysztof Narkowicz's 2016 curve fit of the ACES filmic tone curve.
inline double aces_fitted(double x) {
    const double a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return std::clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Exposure in photographic stops: +1 doubles the light, -1 halves it.
inline double expose(double x, double stops) { return x * std::exp2(stops); }

// Linear 0..1 to the sRGB transfer curve (IEC 61966-2-1), as 0..1.
inline double linear_to_srgb(double x) {
    x = std::clamp(x, 0.0, 1.0);
    return x <= 0.0031308 ? 12.92 * x : 1.055 * std::pow(x, 1.0 / 2.4) - 0.055;
}

// The final step before writing a pixel: 0..1 to an 8-bit value.
inline int to_8bit(double x) { return static_cast<int>(std::clamp(x, 0.0, 1.0) * 255.0 + 0.5); }

}  // namespace tonemap
