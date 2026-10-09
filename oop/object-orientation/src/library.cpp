// library.cpp - object-oriented C++ in one small program: an abstract base
// class, two derived classes, encapsulated state, virtual dispatch, and
// ownership through std::unique_ptr.
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Abstraction: what every item in the library can do, with no data about how.
class Document {
public:
    explicit Document(std::string title) : title_(std::move(title)) {}
    virtual ~Document() = default;              // deleted through a Document*

    const std::string& title() const { return title_; }
    virtual std::string kind() const = 0;       // pure virtual: no Document objects
    virtual std::string progress() const = 0;

private:
    std::string title_;
};

// Encapsulation: the page number can only move to a valid page.
class EBook : public Document {
public:
    EBook(std::string title, int pages) : Document(std::move(title)), pages_(pages)
    {
        if (pages < 1)
            throw std::invalid_argument("an e-book needs at least one page");
    }

    void next_page()     { if (current_ < pages_) ++current_; }
    void previous_page() { if (current_ > 1) --current_; }
    void go_to(int page)
    {
        if (page < 1 || page > pages_)
            throw std::out_of_range("page " + std::to_string(page) + " of " + std::to_string(pages_));
        current_ = page;
    }

    std::string kind() const override { return "e-book"; }
    std::string progress() const override
    {
        return "page " + std::to_string(current_) + " of " + std::to_string(pages_);
    }

private:
    int pages_;
    int current_ = 1;
};

// Inheritance: an audiobook is a Document with a different notion of progress.
class AudioBook : public Document {
public:
    AudioBook(std::string title, int minutes) : Document(std::move(title)), minutes_(minutes) {}

    void listen(int minutes) { heard_ = std::min(minutes_, heard_ + minutes); }

    std::string kind() const override { return "audiobook"; }
    std::string progress() const override
    {
        return std::to_string(heard_) + " of " + std::to_string(minutes_) + " minutes";
    }

private:
    int minutes_;
    int heard_ = 0;
};

// Polymorphism: one loop, and each object answers in its own way.
void print_shelf(const std::vector<std::unique_ptr<Document>>& shelf)
{
    for (const auto& doc : shelf)
        std::cout << "  " << doc->kind() << ": " << doc->title() << " (" << doc->progress() << ")\n";
}

int main()
{
    auto ebook = std::make_unique<EBook>("A Tour of C++", 320);
    auto audio = std::make_unique<AudioBook>("The Pragmatic Programmer", 540);
    EBook& book = *ebook;                       // the objects stay put when the
    AudioBook& talk = *audio;                   // unique_ptrs move into the shelf

    std::vector<std::unique_ptr<Document>> shelf;
    shelf.push_back(std::move(ebook));
    shelf.push_back(std::move(audio));

    book.go_to(41);
    book.next_page();
    talk.listen(95);

    std::cout << "shelf:\n";
    print_shelf(shelf);

    try {
        book.go_to(999);
    } catch (const std::out_of_range& e) {
        std::cout << "rejected: " << e.what() << '\n';
    }
    std::cout << "still on " << book.progress() << '\n';
}
