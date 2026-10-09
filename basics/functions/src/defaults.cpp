// defaults.cpp - default arguments fill in trailing parameters.
#include <iostream>

// The defaults belong in the declaration, and only there.
int box_volume(int length, int width = 2, int height = 3);

int main() {
    std::cout << box_volume(10, 12, 15) << '\n';   // nothing defaulted
    std::cout << box_volume(10, 12) << '\n';       // height = 3
    std::cout << box_volume(10) << '\n';           // width = 2, height = 3
}

int box_volume(int length, int width, int height) {
    std::cout << length << " x " << width << " x " << height << " = ";
    return length * width * height;
}
