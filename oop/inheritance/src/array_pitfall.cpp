// array_pitfall.cpp - a pointer to Base must not walk an array of Derived.
// p[1] steps sizeof(Base) bytes, which is not where the second Derived starts.
#include <iostream>

struct Point {
    int x = 0, y = 0;
};
struct Point3 : Point {
    int z = 0;
};

void print_all(const Point* points, int n) {
    for (int i = 0; i < n; ++i)
        std::cout << "  (" << points[i].x << ", " << points[i].y << ")\n";
}

int main() {
    Point3 pts[3];
    for (int i = 0; i < 3; ++i) { pts[i].x = i + 1; pts[i].y = (i + 1) * 10; pts[i].z = -1; }

    std::cout << "sizeof(Point) = " << sizeof(Point) << ", sizeof(Point3) = " << sizeof(Point3) << '\n';
    std::cout << "through Point3 objects:\n";
    for (const Point3& p : pts) std::cout << "  (" << p.x << ", " << p.y << ")\n";
    std::cout << "through a Point*:\n";
    print_all(pts, 3);                 // compiles: Point3* converts to Point*
}
