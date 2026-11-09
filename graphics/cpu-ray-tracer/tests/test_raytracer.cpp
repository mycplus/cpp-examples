// Unit tests for raytracer.hpp: intersections, shadows, reflections, tone mapping.
#include "../src/raytracer.hpp"

#include <cmath>
#include <cstdio>

static int failures = 0;
static void check(bool ok, const char* what) {
    std::printf("%s: %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok) ++failures;
}
static bool close_to(double a, double b, double tol = 1e-9) { return std::fabs(a - b) < tol; }

int main() {
    using namespace rt;
    const Sphere unit{{0, 0, -5}, 1, {{1, 1, 1}, 0}};

    // A ray straight at the sphere hits its near side at t = 4.
    auto t = intersect({{0, 0, 0}, {0, 0, -1}}, unit);
    check(t && close_to(*t, 4.0), "head-on ray hits the near surface at t = 4");

    // A ray that passes 1.5 units to the side misses.
    check(!intersect({{1.5, 0, 0}, {0, 0, -1}}, unit), "offset ray misses");

    // A ray pointing away from the sphere misses (no hits behind the origin).
    check(!intersect({{0, 0, 0}, {0, 0, 1}}, unit), "ray pointing away misses");

    // A ray that starts inside the sphere hits the far side.
    t = intersect({{0, 0, -5}, {0, 0, -1}}, unit);
    check(t && close_to(*t, 1.0), "ray from the centre exits at t = 1");

    // A tangent ray just touches the sphere.
    t = intersect({{1, 0, 0}, {0, 0, -1}}, unit);
    check(t && close_to(*t, 5.0), "tangent ray touches at t = 5");

    // The nearest of two spheres wins.
    Scene s;
    s.spheres = {{{0, 0, -10}, 1, {{1, 0, 0}, 0}}, {{0, 0, -5}, 1, {{0, 1, 0}, 0}}};
    s.light_pos = {0, 10, 0};
    s.light_colour = {1, 1, 1};
    s.sky_colour = {0, 0, 0};
    auto hit = s.trace({{0, 0, 0}, {0, 0, -1}});
    check(hit && hit->object == 1 && close_to(hit->t, 4.0), "trace returns the nearest sphere");
    check(hit && close_to(hit->normal.z, 1.0), "normal at the hit faces the camera");

    // The top of the red sphere is lit. The ground just behind it, on the
    // line from the light through the sphere, is in its shadow.
    const Scene ground = demo_scene();
    check(!ground.in_shadow({-2.2, 2.0, 0}, {0, 1, 0}), "top of the red sphere is lit");
    check(ground.in_shadow({-1.6, 0, -0.6}, {0, 1, 0}),
          "ground behind the red sphere, away from the light, is in shadow");
    check(!ground.in_shadow({-4.5, 0, 2.0}, {0, 1, 0}), "ground in front of it is lit");

    // A miss returns the sky colour.
    const Vec3 sky = ground.shade({{0, 5, 0}, {0, 1, 0}});
    check(close_to(sky.x, 0.45) && close_to(sky.y, 0.65) && close_to(sky.z, 0.95), "a miss returns the sky");

    // A perfect mirror facing the camera reflects the ray straight back.
    Scene mirror;
    mirror.spheres = {{{0, 0, -5}, 1, {{1, 1, 1}, 1.0}}};
    mirror.light_pos = {0, 0, 10};
    mirror.light_colour = {0, 0, 0};
    mirror.sky_colour = {0.25, 0.5, 0.75};
    const Vec3 m = mirror.shade({{0, 0, 0}, {0, 0, -1}});
    check(close_to(m.x, 0.25) && close_to(m.y, 0.5) && close_to(m.z, 0.75),
          "a perfect mirror shows the sky behind the camera");

    // Tone mapping: 0 stays black, values above 1 stay below white, order is kept.
    check(to_byte(0.0) == 0, "black maps to 0");
    check(to_byte(1000.0) == 255 && to_byte(4.0) < 255, "only very bright values reach 255");
    check(to_byte(0.2) < to_byte(0.5) && to_byte(0.5) < to_byte(2.0), "tone mapping keeps order");

    std::printf(failures ? "%d test(s) failed\n" : "all tests passed\n", failures);
    return failures ? 1 : 0;
}
