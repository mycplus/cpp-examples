// ambiguous_call.cpp - must not compile: 2.5 is a double, and converting it
// to int or to float ranks the same, so neither overload is better.
void scale(int factor);
void scale(float factor);

int main() {
    scale(2.5);
}
