// validate_json: report whether each file holds one valid JSON text (RFC 8259).
// Usage: validate_json FILE...
#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

using json = nlohmann::json;

// Returns an empty string for valid JSON, otherwise the parser's message.
std::string check_json(const std::string& text)
{
    try {
        // Parse into a value we throw away: the point is the exception.
        [[maybe_unused]] json value = json::parse(text);
        return {};
    } catch (const json::parse_error& e) {
        // e.byte is the 1-based byte offset where parsing stopped.
        return e.what();
    }
}

int main(int argc, char* argv[])
{
    int invalid = 0;
    for (int i = 1; i < argc; ++i) {
        std::ifstream file(argv[i], std::ios::binary);
        if (!file) {
            std::cerr << argv[i] << ": cannot open\n";
            return 2;
        }
        const std::string text{std::istreambuf_iterator<char>(file), {}};
        const std::string error = check_json(text);
        std::cout << argv[i] << (error.empty() ? ": valid" : ": invalid (" + error + ")") << '\n';
        invalid += error.empty() ? 0 : 1;
    }
    return invalid == 0 ? 0 : 1;
}
