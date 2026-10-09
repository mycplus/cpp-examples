// temporary_to_ref.cpp - must not compile: a non-const reference parameter
// cannot bind to a temporary value.
void add_bonus(int& score) { score += 10; }

int main() {
    add_bonus(5);
}
