// dangling_return.cpp - returning a reference to a local variable.
// Compiles with a warning; calling it is undefined behavior.
#include <string>

const std::string& greeting(const std::string& name) {
    std::string text = "Hello, " + name;
    return text;   // text is destroyed when the function returns
}

int main() {
    return greeting("Ada").empty() ? 1 : 0;
}
