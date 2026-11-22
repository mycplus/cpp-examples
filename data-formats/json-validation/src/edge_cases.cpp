// edge_cases: show what nlohmann/json does with inputs that JSON parsers
// handle differently. Usage: edge_cases FILE...
#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

using json = nlohmann::json;

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        std::ifstream file(argv[i], std::ios::binary);
        const std::string text{std::istreambuf_iterator<char>(file), {}};
        try {
            const json value = json::parse(text);
            // Very deep values are not printed: dump() recurses once per level.
            std::cout << argv[i] << ": accepted"
                      << (text.size() > 1000 ? std::string() : ", value " + value.dump()) << '\n';
        } catch (const json::parse_error& e) {
            std::cout << argv[i] << ": rejected (" << e.what() << ")\n";
        }
    }
    // With ignore_comments = true, nlohmann/json also reads JSON with comments.
    const std::string with_comment = "{\n  \"a\": 1 // port\n}";
    std::cout << "comment, ignore_comments=true: "
              << json::parse(with_comment, nullptr, true, true).dump() << '\n';
    return 0;
}
