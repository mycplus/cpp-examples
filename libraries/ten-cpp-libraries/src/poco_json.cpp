// poco_json.cpp - parse and query JSON with Poco::JSON, and split a URL with
// Poco::URI. Links PocoJSON and PocoFoundation.
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>

#include <iostream>

int main()
{
    const std::string json = R"({"course":"C++","students":[{"name":"Ada","score":91},)"
                             R"({"name":"Linus","score":78}],"open":true})";

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var parsed = parser.parse(json);
    auto root = parsed.extract<Poco::JSON::Object::Ptr>();

    std::cout << "course: " << root->getValue<std::string>("course") << '\n';
    std::cout << "open: " << std::boolalpha << root->getValue<bool>("open") << '\n';
    auto students = root->getArray("students");
    for (std::size_t i = 0; i < students->size(); ++i) {
        auto s = students->getObject(static_cast<unsigned>(i));
        std::cout << s->getValue<std::string>("name") << ' ' << s->getValue<int>("score") << '\n';
    }

    Poco::URI uri("https://www.example.com:8443/search?q=c%2B%2B&page=2");
    std::cout << "host " << uri.getHost() << ", port " << uri.getPort()
              << ", path " << uri.getPath() << ", query " << uri.getQuery() << '\n';
}
