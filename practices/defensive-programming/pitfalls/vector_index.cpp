// operator[] does not check the index; at() does, and throws.
// Reading v[3] below would be undefined behavior, so this program only
// shows the checked version. Building with -D_GLIBCXX_ASSERTIONS (GCC,
// Clang with libstdc++) or with MSVC's debug library makes [] check too.
#include <iostream>
#include <stdexcept>
#include <vector>

int main()
{
    const std::vector<int> v{10, 20, 30};
    for (std::size_t i = 0; i <= v.size(); ++i) { // one step too far, on purpose
        try {
            const int value = v.at(i); // read first: a throw must not leave half a line
            std::cout << "v.at(" << i << ") = " << value << '\n';
        } catch (const std::out_of_range&) {
            std::cout << "v.at(" << i << ") threw std::out_of_range\n";
        }
    }
    return 0;
}
