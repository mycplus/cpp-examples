// eh_demo.cpp - a program that throws and catches a C++ exception.
// Built for x64 with Visual Studio 2019 or later, it imports
// __CxxFrameHandler4 from VCRUNTIME140_1.dll.
#include <cstdio>
#include <stdexcept>
#include <string>

static int parse_port(const std::string& text)
{
    const int port = std::stoi(text);       // throws std::invalid_argument for "abc"
    if (port < 1 || port > 65535)
        throw std::out_of_range("port must be 1-65535");
    return port;
}

int main()
{
    for (const char* input : {"8080", "abc", "70000"}) {
        try {
            std::printf("%s -> port %d\n", input, parse_port(input));
        } catch (const std::invalid_argument&) {
            std::printf("%s -> not a number\n", input);
        } catch (const std::out_of_range& e) {
            std::printf("%s -> %s\n", input, e.what());
        }
    }
}
