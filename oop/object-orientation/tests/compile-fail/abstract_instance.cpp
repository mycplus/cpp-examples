// abstract_instance.cpp - must NOT compile: a class with a pure virtual
// function is abstract, so no object of that exact type can be created.
#include <string>

class Document {
public:
    virtual ~Document() = default;
    virtual std::string kind() const = 0;
};

int main()
{
    Document d;
    return static_cast<int>(d.kind().size());
}
