// fragile_base.cpp - a derived class that depends on how its base class is
// implemented. Tag::add_all happens to call add(), so the count doubles.
#include <iostream>
#include <set>
#include <string>
#include <vector>

class Tags {
public:
    virtual ~Tags() = default;
    virtual void add(const std::string& tag) { tags_.insert(tag); }
    virtual void add_all(const std::vector<std::string>& tags) {
        for (const auto& t : tags) add(t);         // an implementation detail
    }
    std::size_t size() const { return tags_.size(); }

private:
    std::set<std::string> tags_;
};

class CountingTags : public Tags {
public:
    void add(const std::string& tag) override { ++attempts_; Tags::add(tag); }
    void add_all(const std::vector<std::string>& tags) override {
        attempts_ += static_cast<int>(tags.size());
        Tags::add_all(tags);                       // calls add() three more times
    }
    int attempts() const { return attempts_; }

private:
    int attempts_ = 0;
};

int main() {
    CountingTags t;
    t.add_all({"urgent", "billing", "eu"});
    std::cout << "tags stored: " << t.size() << ", attempts counted: " << t.attempts() << '\n';
}
