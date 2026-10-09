// default_not_trailing.cpp - must not compile: a parameter with a default
// is followed by one without.
int box_volume(int length = 1, int width, int height);

int main() { return 0; }
