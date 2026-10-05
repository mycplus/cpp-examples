// file_handle.cpp - std::unique_ptr managing a C FILE* with a custom deleter.
#include <cstdio>
#include <memory>

struct FileCloser {
    void operator()(std::FILE* f) const noexcept
    {
        std::puts("  fclose() called");
        std::fclose(f);
    }
};

using File = std::unique_ptr<std::FILE, FileCloser>;

// Returns an empty File if fopen fails; the deleter is never called on null.
File open_file(const char* path, const char* mode)
{
    return File(std::fopen(path, mode));
}

int main()
{
    const char* path = "smart_pointers_demo.txt";

    if (File out = open_file(path, "w")) {
        std::fputs("written through a unique_ptr<FILE>\n", out.get());
    }   // closed here, before the file is reopened

    File in = open_file(path, "r");
    if (!in) {
        std::perror(path);
        return 1;
    }
    char line[64];
    if (std::fgets(line, sizeof line, in.get()))
        std::printf("  read back: %s", line);

    File missing = open_file("no/such/dir/file.txt", "r");
    std::printf("  missing file handle is %s\n", missing ? "open" : "null");

    in.reset();                  // close now rather than at end of scope
    std::remove(path);
    std::puts("  end of main");
}
