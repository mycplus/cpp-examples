// dangling_temporary.cpp - returning a const reference parameter is safe while
// the caller's full expression lasts, and dangles if the caller keeps it.
#include <iostream>
#include <string>

const std::string& longer(const std::string& a, const std::string& b)
{
    return a.size() >= b.size() ? a : b;
}

int main()
{
    std::string name = "Lovelace";

    // Safe: the temporary std::string lives until the end of this statement.
    std::cout << "in one expression: " << longer(name, std::string("Ada")) << '\n';

    // Dangles: the temporary is destroyed at the semicolon, `kept` still refers to it.
    const std::string& kept = longer(std::string("Babbage, Charles"), name);
    std::cout << "kept reference:    " << kept << '\n';
}
