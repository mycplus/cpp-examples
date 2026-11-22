// validate_user: check that each file is a JSON user record with the expected
// fields and types. The rules match cases/schema/user.schema.json.
// Usage: validate_user FILE...
#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using json = nlohmann::json;

bool looks_like_email(const std::string& s)
{
    const auto at = s.find('@');
    const auto dot = s.rfind('.');
    return at != std::string::npos && at > 0 && s.find('@', at + 1) == std::string::npos &&
           dot != std::string::npos && dot > at + 1 && dot + 1 < s.size() &&
           s.find_first_of(" \t\r\n") == std::string::npos;
}

std::vector<std::string> check_user(const json& user)
{
    std::vector<std::string> errors;
    if (!user.is_object()) {
        return {"/: must be an object"};
    }
    for (const char* field : {"id", "name", "email", "active", "roles"}) {
        if (!user.contains(field)) {
            errors.push_back("/: missing required field \"" + std::string(field) + "\"");
        }
    }
    const std::set<std::string> known{"id", "name", "email", "active", "roles",
                                      "manager", "score", "address"};
    for (const auto& item : user.items()) {
        if (known.count(item.key()) == 0) {
            errors.push_back("/: unexpected field \"" + item.key() + "\"");
        }
    }
    if (user.contains("id") && !(user["id"].is_number_integer() && user["id"].get<long long>() >= 1)) {
        errors.push_back("/id: must be an integer >= 1");
    }
    if (user.contains("name") && !(user["name"].is_string() && !user["name"].get<std::string>().empty())) {
        errors.push_back("/name: must be a non-empty string");
    }
    if (user.contains("email") && !(user["email"].is_string() && looks_like_email(user["email"].get<std::string>()))) {
        errors.push_back("/email: must be an email address");
    }
    if (user.contains("active") && !user["active"].is_boolean()) {
        errors.push_back("/active: must be true or false");
    }
    if (user.contains("roles")) {
        const json& roles = user["roles"];
        if (!roles.is_array()) {
            errors.push_back("/roles: must be an array");
        } else {
            const std::set<std::string> allowed{"admin", "editor", "viewer"};
            for (std::size_t i = 0; i < roles.size(); ++i) {
                if (!roles[i].is_string() || allowed.count(roles[i].get<std::string>()) == 0) {
                    errors.push_back("/roles/" + std::to_string(i) + ": must be admin, editor or viewer");
                } else if (std::find(roles.begin(), roles.begin() + static_cast<std::ptrdiff_t>(i), roles[i]) !=
                           roles.begin() + static_cast<std::ptrdiff_t>(i)) {
                    errors.push_back("/roles/" + std::to_string(i) + ": duplicate role");
                }
            }
        }
    }
    if (user.contains("manager") && !(user["manager"].is_string() || user["manager"].is_null())) {
        errors.push_back("/manager: must be a string or null");
    }
    if (user.contains("score")) {
        const json& score = user["score"];
        if (!score.is_number() || score.get<double>() < 0 || score.get<double>() > 100) {
            errors.push_back("/score: must be a number from 0 to 100");
        }
    }
    if (user.contains("address")) {
        const json& address = user["address"];
        if (!address.is_object() || !address.contains("city") || !address["city"].is_string()) {
            errors.push_back("/address: must be an object with a string \"city\"");
        }
    }
    return errors;
}

int main(int argc, char* argv[])
{
    int invalid = 0;
    for (int i = 1; i < argc; ++i) {
        std::ifstream file(argv[i]);
        if (!file) {
            std::cerr << argv[i] << ": cannot open\n";
            return 2;
        }
        // allow_exceptions = false: a syntax error gives a "discarded" value.
        const json user = json::parse(file, nullptr, false);
        std::vector<std::string> errors =
            user.is_discarded() ? std::vector<std::string>{"not valid JSON"} : check_user(user);
        std::cout << argv[i] << (errors.empty() ? ": valid" : ": invalid") << '\n';
        for (const auto& e : errors) {
            std::cout << "  " << e << '\n';
        }
        invalid += errors.empty() ? 0 : 1;
    }
    return invalid == 0 ? 0 : 1;
}
