// type_census.cpp - asks the compiler which category each type belongs to.
// The type traits in <type_traits> answer the same questions the C++
// standard uses to divide types into fundamental and compound.
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>

struct Point { int x; int y; };

class Account {
public:
    double balance() const { return balance_; }
private:
    double balance_ = 0;
};

union Number { int i; float f; };

enum class Color { red, green, blue };

template <typename T>
const char* category()
{
    if constexpr (std::is_fundamental_v<T>) return "fundamental";
    else if constexpr (std::is_array_v<T>) return "array";
    else if constexpr (std::is_function_v<T>) return "function";
    else if constexpr (std::is_lvalue_reference_v<T>) return "lvalue reference";
    else if constexpr (std::is_rvalue_reference_v<T>) return "rvalue reference";
    else if constexpr (std::is_member_object_pointer_v<T>) return "pointer to data member";
    else if constexpr (std::is_member_function_pointer_v<T>) return "pointer to member function";
    else if constexpr (std::is_pointer_v<T>) return "pointer";
    else if constexpr (std::is_enum_v<T>) return "enumeration";
    else if constexpr (std::is_union_v<T>) return "union";
    else if constexpr (std::is_class_v<T>) return "class";
    else return "unknown";
}

template <typename T>
void row(const char* name)
{
    std::cout << std::left << std::setw(30) << name
              << std::setw(13) << (std::is_compound_v<T> ? "compound" : "fundamental")
              << category<T>() << '\n';
}

int main()
{
    std::cout << std::left << std::setw(30) << "type" << std::setw(13) << "kind" << "category\n";
    row<int>("int");
    row<double>("double");
    row<bool>("bool");
    row<void>("void");
    row<std::nullptr_t>("std::nullptr_t");
    row<int[4]>("int[4]");
    row<int(int, int)>("int(int, int)");
    row<int*>("int*");
    row<int&>("int&");
    row<int&&>("int&&");
    row<Point>("Point (struct)");
    row<Account>("Account (class)");
    row<Number>("Number (union)");
    row<Color>("Color (enum class)");
    row<int Point::*>("int Point::*");
    row<double (Account::*)() const>("double (Account::*)() const");
    row<std::string>("std::string");
}
