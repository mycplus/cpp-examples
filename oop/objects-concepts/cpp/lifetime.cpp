// lifetime.cpp - an object that owns a resource releases it when its scope
// ends: the destructor runs at a known point (RAII).
#include <iostream>
#include <string>
#include <utility>

class LogFile {
public:
    explicit LogFile(std::string name) : name_(std::move(name)) { std::cout << "open " << name_ << '\n'; }
    ~LogFile() { std::cout << "close " << name_ << '\n'; }
    LogFile(const LogFile&) = delete;              // one owner of the file
    LogFile& operator=(const LogFile&) = delete;

    void write(const std::string& line) { std::cout << "write " << line << '\n'; }

private:
    std::string name_;
};

int main() {
    {
        LogFile log("app.log");
        log.write("started");
    }                                              // ~LogFile runs here
    std::cout << "after the block\n";
}
