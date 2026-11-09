// A small Whitted-style ray tracer: spheres, one point light, hard shadows
// and mirror reflections. Companion code for the MYCPLUS article
// on graphics technologies in AAA game development with C++.
#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

namespace rt {

struct Vec3 {
    double x = 0, y = 0, z = 0;
};
inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(Vec3 a, double s) { return {a.x * s, a.y * s, a.z * s}; }
inline Vec3 operator*(Vec3 a, Vec3 b) { return {a.x * b.x, a.y * b.y, a.z * b.z}; }
inline double dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
inline Vec3 normalize(Vec3 v) { return v * (1.0 / std::sqrt(dot(v, v))); }

// A ray is origin + t * dir, with dir of unit length.
struct Ray {
    Vec3 origin, dir;
};

struct Material {
    Vec3 albedo;          // diffuse colour
    double reflectivity;  // 0 = matte, 1 = perfect mirror
};

struct Sphere {
    Vec3 centre;
    double radius;
    Material material;
};

struct Hit {
    double t;        // distance along the ray
    Vec3 point;      // where the ray hit
    Vec3 normal;     // unit surface normal at the hit
    int object;      // index of the sphere that was hit
};

// Solve |origin + t*dir - centre|^2 = r^2 for the nearest t > epsilon.
inline std::optional<double> intersect(const Ray& ray, const Sphere& s) {
    const Vec3 oc = ray.origin - s.centre;
    const double b = dot(oc, ray.dir);
    const double c = dot(oc, oc) - s.radius * s.radius;
    const double disc = b * b - c;
    if (disc < 0) return std::nullopt;  // the ray misses the sphere
    const double root = std::sqrt(disc);
    const double eps = 1e-6;
    if (const double t = -b - root; t > eps) return t;  // hit from outside
    if (const double t = -b + root; t > eps) return t;  // origin inside
    return std::nullopt;
}

struct Scene {
    std::vector<Sphere> spheres;
    Vec3 light_pos;
    Vec3 light_colour;
    Vec3 sky_colour;

    std::optional<Hit> trace(const Ray& ray) const {
        std::optional<Hit> best;
        for (int i = 0; i < static_cast<int>(spheres.size()); ++i) {
            if (auto t = intersect(ray, spheres[i]); t && (!best || *t < best->t)) {
                const Vec3 p = ray.origin + ray.dir * *t;
                best = Hit{*t, p, normalize(p - spheres[i].centre), i};
            }
        }
        return best;
    }

    // True if anything blocks the straight line from point to the light.
    bool in_shadow(Vec3 point, Vec3 normal) const {
        const Vec3 to_light = light_pos - point;
        const double dist = std::sqrt(dot(to_light, to_light));
        const Ray shadow_ray{point + normal * 1e-4, to_light * (1.0 / dist)};
        const auto hit = trace(shadow_ray);
        return hit && hit->t < dist;
    }

    // Returns linear (HDR) radiance: values can be greater than 1.
    Vec3 shade(const Ray& ray, int depth = 0) const {
        const auto hit = trace(ray);
        if (!hit) return sky_colour;
        const Material& m = spheres[hit->object].material;

        Vec3 colour = m.albedo * 0.05;  // a little ambient light
        if (!in_shadow(hit->point, hit->normal)) {
            const Vec3 l = normalize(light_pos - hit->point);
            const double lambert = std::max(0.0, dot(hit->normal, l));
            colour = colour + m.albedo * light_colour * lambert;
        }
        if (m.reflectivity > 0 && depth < 3) {
            const Vec3 r = ray.dir - hit->normal * (2 * dot(ray.dir, hit->normal));
            const Ray bounce{hit->point + hit->normal * 1e-4, normalize(r)};
            colour = colour * (1 - m.reflectivity) + shade(bounce, depth + 1) * m.reflectivity;
        }
        return colour;
    }
};

// The scene the demo renders: a ground sphere and three spheres on it.
inline Scene demo_scene() {
    Scene s;
    s.spheres = {
        {{0, -1000, 0}, 1000, {{0.55, 0.55, 0.50}, 0.0}},  // ground
        {{-2.2, 1, 0}, 1, {{0.90, 0.20, 0.15}, 0.0}},      // red, matte
        {{0, 1, -1}, 1, {{0.90, 0.90, 0.90}, 0.8}},        // mirror
        {{2.2, 1, 0}, 1, {{0.15, 0.35, 0.90}, 0.1}},       // blue
    };
    s.light_pos = {-6, 10, 6};
    s.light_colour = {1.6, 1.5, 1.4};  // brighter than 1.0: this is HDR
    s.sky_colour = {0.45, 0.65, 0.95};
    return s;
}

// A pinhole camera at `eye` looking at `target`; (u, v) in [0, 1].
inline Ray camera_ray(Vec3 eye, Vec3 target, double fov_deg, double aspect,
                      double u, double v) {
    const Vec3 fwd = normalize(target - eye);
    const Vec3 right = normalize(cross(fwd, Vec3{0, 1, 0}));
    const Vec3 up = cross(right, fwd);
    const double h = std::tan(fov_deg * 3.14159265358979323846 / 360.0);
    const Vec3 d = fwd + right * ((2 * u - 1) * h * aspect) + up * ((1 - 2 * v) * h);
    return {eye, normalize(d)};
}

// HDR to an 8-bit display value: Reinhard tone mapping, then gamma 2.2.
inline int to_byte(double linear) {
    const double mapped = linear / (1.0 + linear);
    const double encoded = std::pow(mapped, 1.0 / 2.2);
    return std::clamp(static_cast<int>(encoded * 255.0 + 0.5), 0, 255);
}

}  // namespace rt
