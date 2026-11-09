// Renders the demo scene to a binary PPM image and prints a summary.
// Usage: ray_tracer [output.ppm] [width] [height]
#include "raytracer.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

int main(int argc, char** argv) {
    const std::string out = argc > 1 ? argv[1] : "render.ppm";
    const int width = argc > 2 ? std::atoi(argv[2]) : 640;
    const int height = argc > 3 ? std::atoi(argv[3]) : 360;
    if (width <= 0 || height <= 0) {
        std::fprintf(stderr, "width and height must be positive\n");
        return 1;
    }

    const rt::Scene scene = rt::demo_scene();
    const rt::Vec3 eye{0, 2.2, 7}, target{0, 0.9, 0};
    const double aspect = static_cast<double>(width) / height;

    std::ofstream file(out, std::ios::binary);
    if (!file) {
        std::fprintf(stderr, "cannot write %s\n", out.c_str());
        return 1;
    }
    file << "P6\n" << width << ' ' << height << "\n255\n";

    std::array<long, 4> hits{};  // primary-ray hits per sphere
    long sky = 0, shadowed = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const double u = (x + 0.5) / width, v = (y + 0.5) / height;
            const rt::Ray ray = rt::camera_ray(eye, target, 50.0, aspect, u, v);

            if (const auto hit = scene.trace(ray)) {
                ++hits[static_cast<std::size_t>(hit->object)];
                if (scene.in_shadow(hit->point, hit->normal)) ++shadowed;
            } else {
                ++sky;
            }

            const rt::Vec3 c = scene.shade(ray);
            const char rgb[3] = {static_cast<char>(rt::to_byte(c.x)),
                                 static_cast<char>(rt::to_byte(c.y)),
                                 static_cast<char>(rt::to_byte(c.z))};
            file.write(rgb, 3);
        }
    }

    std::printf("image: %dx%d\n", width, height);
    std::printf("sky pixels: %ld\n", sky);
    const char* names[4] = {"ground", "red", "mirror", "blue"};
    for (std::size_t i = 0; i < hits.size(); ++i)
        std::printf("%s pixels: %ld\n", names[i], hits[i]);
    std::printf("pixels in shadow: %ld\n", shadowed);
    std::printf("wrote %s\n", out.c_str());
    return 0;
}
