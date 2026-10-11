// leak.cpp - returning a reference to private data hands it to the caller.
// LeakyTeam's limit of three members can be broken from outside; Team's cannot.
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr std::size_t kLimit = 3;

class LeakyTeam {
public:
    void add(const std::string& name) {
        if (members_.size() == kLimit) throw std::length_error("team is full");
        members_.push_back(name);
    }
    std::vector<std::string>& members() { return members_; }  // the private vector itself

private:
    std::vector<std::string> members_;
};

class Team {
public:
    void add(const std::string& name) {
        if (members_.size() == kLimit) throw std::length_error("team is full");
        members_.push_back(name);
    }
    const std::vector<std::string>& members() const { return members_; }  // read-only

private:
    std::vector<std::string> members_;
};

int main() {
    LeakyTeam leaky;
    for (const char* n : {"Ana", "Ben", "Cy"}) leaky.add(n);
    std::cout << "leaky: added Ana, Ben, Cy -> " << leaky.members().size() << " members\n";
    leaky.members().push_back("Dee");
    leaky.members().push_back("Eve");
    std::cout << "leaky: caller appended twice -> " << leaky.members().size()
              << " members (limit is " << kLimit << ")\n";

    Team team;
    for (const char* n : {"Ana", "Ben", "Cy"}) team.add(n);
    std::cout << "safe: added Ana, Ben, Cy -> " << team.members().size() << " members\n";
    // team.members().push_back("Dee");  // does not compile: the reference is const
    std::vector<std::string> mine = team.members();  // an explicit copy is the caller's own
    mine.push_back("Dee");
    std::cout << "safe: caller's copy has " << mine.size() << ", team still has "
              << team.members().size() << " members\n";
}
