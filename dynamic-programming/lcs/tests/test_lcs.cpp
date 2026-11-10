// test_lcs.cpp - checks lcs(), lcs_length(), lcs_pairs() and diff() against
// brute force and against each other. Includes the program's source so the
// listing stays one complete file.
#define main lcs_program_main
#include "../src/lcs.cpp"
#undef main

#include <cstdint>
#include <cstdio>

namespace {

int failures = 0;

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::printf("FAILED line %d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

std::uint64_t rng_state = 2026;
std::uint64_t next_random()  // splitmix64
{
    std::uint64_t z = (rng_state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

std::string random_string(std::size_t max_len, unsigned alphabet)
{
    std::string s(next_random() % (max_len + 1), ' ');
    for (char& c : s)
        c = static_cast<char>('A' + next_random() % alphabet);
    return s;
}

bool is_subsequence(std::string_view s, std::string_view t)
{
    std::size_t k = 0;
    for (char c : t)
        if (k < s.size() && s[k] == c)
            ++k;
    return k == s.size();
}

std::size_t brute_force_length(const std::string& a, const std::string& b)
{
    std::size_t best = 0;
    for (unsigned long mask = 0; mask < (1UL << a.size()); ++mask) {
        std::string sub;
        for (std::size_t i = 0; i < a.size(); ++i)
            if (mask & (1UL << i))
                sub += a[i];
        if (sub.size() > best && is_subsequence(sub, b))
            best = sub.size();
    }
    return best;
}

bool diff_rebuilds(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    std::vector<std::string> ra, rb;
    std::size_t changes = 0;
    for (const DiffLine& d : diff(a, b)) {
        if (d.op != '+')
            ra.push_back(d.text);
        if (d.op != '-')
            rb.push_back(d.text);
        changes += d.op != ' ';
    }
    return ra == a && rb == b && changes == a.size() + b.size() - 2 * lcs_pairs(a, b).size();
}

}  // namespace

int main()
{
    CHECK(lcs("", "") == "");
    CHECK(lcs("ABC", "DEF") == "");
    CHECK(lcs("ABCBDAB", "BDCABA").size() == 4);
    CHECK(lcs_length("AGGTAB", "GXTXAYB") == 4);

    int pairs = 0, pairs_ok = 0;
    for (unsigned alphabet = 2; alphabet <= 6; ++alphabet)
        for (int trial = 0; trial < 2000; ++trial) {
            const std::string a = random_string(12, alphabet), b = random_string(12, alphabet);
            const std::size_t want = brute_force_length(a, b);
            const std::string r = lcs(a, b);
            ++pairs;
            if (r.size() == want && is_subsequence(r, a) && is_subsequence(r, b) &&
                lcs_length(a, b) == want && lcs_length(b, a) == want)
                ++pairs_ok;
            else
                std::printf("  a=\"%s\" b=\"%s\": got \"%s\", wanted length %zu\n", a.c_str(),
                            b.c_str(), r.c_str(), want);
        }
    std::printf("short random pairs agreeing with brute force: %d of %d\n", pairs_ok, pairs);

    // The generic version on ints gives the same length as on equivalent strings.
    int generic_ok = 0;
    for (int trial = 0; trial < 500; ++trial) {
        const std::string a = random_string(40, 3), b = random_string(40, 3);
        std::vector<int> va(a.begin(), a.end()), vb(b.begin(), b.end());
        generic_ok += lcs_pairs(va, vb).size() == lcs_length(a, b);
    }
    std::printf("int sequences matching the string result: %d of 500\n", generic_ok);

    static const char* const words[] = {"alpha", "beta", "gamma", "delta"};
    int diffs_ok = 0;
    for (int trial = 0; trial < 500; ++trial) {
        std::vector<std::string> a(next_random() % 12), b(next_random() % 12);
        for (auto& s : a)
            s = words[next_random() % 4];
        for (auto& s : b)
            s = words[next_random() % 4];
        diffs_ok += diff_rebuilds(a, b);
    }
    std::printf("random diffs that rebuild both inputs: %d of 500\n", diffs_ok);
    std::printf("unit checks failed: %d\n", failures);
    return failures == 0 && pairs_ok == pairs && generic_ok == 500 && diffs_ok == 500 ? 0 : 1;
}
