// default_redefined.cpp - must not compile: the default argument is repeated
// in the definition.
int box_volume(int length, int width = 2, int height = 3);

int box_volume(int length, int width = 2, int height = 3) {
    return length * width * height;
}

int main() { return box_volume(1) == 6 ? 0 : 1; }
