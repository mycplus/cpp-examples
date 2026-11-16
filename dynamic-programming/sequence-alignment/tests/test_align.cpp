// test_align.cpp - checks align() and align_score() against known results,
// against a brute force that scores every possible alignment, and against
// each other. Includes the program's source so the listing stays one file.
#define main align_program_main
#include "../src/align.cpp"
#undef main

#include <cstdint>
#include <cstdio>
#include <iterator>

namespace {

int failures = 0;

void check(bool ok, const std::string& what, int line)
{
    if (!ok) {
        std::printf("FAILED line %d: %s\n", line, what.c_str());
        ++failures;
    }
}
#define CHECK(cond, what) check((cond), (what), __LINE__)

std::uint64_t rng_state = 2026;
std::uint64_t next_random()  // splitmix64
{
    std::uint64_t z = (rng_state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
int below(int n) { return static_cast<int>(next_random() % static_cast<std::uint64_t>(n)); }

std::string random_string(std::string_view letters, int max_len)
{
    std::string s(static_cast<std::size_t>(below(max_len + 1)), ' ');
    for (char& c : s)
        c = letters[static_cast<std::size_t>(below(static_cast<int>(letters.size())))];
    return s;
}

// The score of an alignment from its columns: pairs from the table, and
// open + length * extend for every maximal run of gaps in one row.
int score_columns(const Alignment& al, const Scoring& sc)
{
    int score = 0;
    for (std::size_t k = 0; k < al.top.size(); ++k) {
        if (al.top[k] == '-' || al.bottom[k] == '-') {
            const std::string& row = al.top[k] == '-' ? al.top : al.bottom;
            if (k == 0 || row[k - 1] != '-')
                score -= sc.open();
            score -= sc.extend();
        } else {
            score += sc.score(sc.code(al.top[k]), sc.code(al.bottom[k]));
        }
    }
    return score;
}

// Brute force over every alignment: the best of the three choices for the
// last column, computed column by column with no table.
int brute_global(std::string_view a, std::string_view b, const Scoring& sc)
{
    int best = INT_MIN;
    Alignment al;
    auto go = [&](auto&& self, std::size_t n, std::size_t m) -> void {
        if (n == 0 && m == 0) {
            Alignment fwd{std::string(al.top.rbegin(), al.top.rend()),
                          std::string(al.bottom.rbegin(), al.bottom.rend())};
            best = std::max(best, score_columns(fwd, sc));
            return;
        }
        const char choices[3][2] = {{1, 1}, {1, 0}, {0, 1}};
        for (const auto& c : choices) {
            if ((c[0] && n == 0) || (c[1] && m == 0))
                continue;
            al.top += c[0] ? a[n - 1] : '-';
            al.bottom += c[1] ? b[m - 1] : '-';
            self(self, n - static_cast<std::size_t>(c[0]), m - static_cast<std::size_t>(c[1]));
            al.top.pop_back();
            al.bottom.pop_back();
        }
    };
    go(go, a.size(), b.size());
    return best;
}

// A local alignment is a global alignment of two substrings, or empty.
int brute_local(std::string_view a, std::string_view b, const Scoring& sc)
{
    int best = 0;
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t i2 = i + 1; i2 <= a.size(); ++i2)
            for (std::size_t j = 0; j < b.size(); ++j)
                for (std::size_t j2 = j + 1; j2 <= b.size(); ++j2)
                    best = std::max(best, brute_global(a.substr(i, i2 - i), b.substr(j, j2 - j), sc));
    return best;
}

std::string remove_gaps(const std::string& row)
{
    std::string s;
    std::copy_if(row.begin(), row.end(), std::back_inserter(s), [](char c) { return c != '-'; });
    return s;
}

// align() and align_score() on one pair, checked against `want`.
bool check_pair(std::string_view s1, std::string_view s2, const Scoring& sc, Mode mode, int want)
{
    const std::string what = std::string(mode == Mode::local ? "local" : "global") + " \""
                             + std::string(s1.substr(0, 30)) + "\" \"" + std::string(s2.substr(0, 30)) + "\"";
    const Alignment al = align(s1, s2, sc, mode);
    bool ok = al.top.size() == al.bottom.size()
              && remove_gaps(al.top) == s1.substr(al.start1, al.end1 - al.start1)
              && remove_gaps(al.bottom) == s2.substr(al.start2, al.end2 - al.start2)
              && score_columns(al, sc) == al.score && al.score == want
              && align_score(s1, s2, sc, mode) == want;
    for (std::size_t k = 0; k < al.top.size(); ++k)
        ok = ok && !(al.top[k] == '-' && al.bottom[k] == '-');
    if (mode == Mode::global)
        ok = ok && al.start1 == 0 && al.end1 == s1.size() && al.start2 == 0 && al.end2 == s2.size();
    CHECK(ok, what + ": score " + std::to_string(al.score) + ", wanted " + std::to_string(want));
    return ok;
}

void known_results()
{
    const Scoring dna_linear("ACGT", dna.data(), 0, 2);
    const Scoring dna_affine("ACGT", dna.data(), 8, 1);
    const Scoring b50(blosum_letters, blosum50.data(), 0, 8);
    const Scoring b62(blosum_letters, blosum62.data(), 11, 1);
    check_pair("TGTTACGG", "GGTTGACTA", dna_linear, Mode::local, 13);
    check_pair("TGTTACGG", "GGTTGACTA", dna_linear, Mode::global, 4);
    check_pair("HEAGAWGHEE", "PAWHEAE", b50, Mode::global, 1);
    check_pair("HEAGAWGHEE", "PAWHEAE", b50, Mode::local, 28);
    check_pair("GAAAAAGATAGAGC", "GAAAAAGAAAC", dna_linear, Mode::global, 27);
    check_pair("GAAAAAGATAGAGC", "GAAAAAGAAAC", dna_affine, Mode::global, 16);
    check_pair("", "", dna_linear, Mode::global, 0);
    check_pair("", "ACG", dna_linear, Mode::global, -6);
    check_pair("ACG", "", dna_affine, Mode::global, -11);
    check_pair("AAAA", "CCCC", dna_linear, Mode::local, 0);
    check_pair("acgt", "ACGT", dna_linear, Mode::local, 12);
    check_pair("WWW*", "WWW*", b62, Mode::local, 34);

    bool threw = false;
    try {
        align("ACGU", "ACG", dna_linear, Mode::global);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw, "U is not in ACGT");
    threw = false;
    try {
        Scoring bad("ACGT", dna.data(), -1, 2);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw, "negative gap open");
    threw = false;
    try {
        const Scoring huge("ACGT", dna.data(), 1 << 20, 1 << 20);
        align(std::string(200, 'A'), "A", huge, Mode::global);
    } catch (const std::length_error&) {
        threw = true;
    }
    CHECK(threw, "scores could overflow");
}

// Random asymmetric scoring over three letters, so that looking the two
// letters up in the wrong order is caught.
void against_brute_force(int pairs)
{
    int agree = 0;
    for (int t = 0; t < pairs; ++t) {
        std::array<int, 9> table{};
        for (int& v : table)
            v = below(11) - 5;
        const Scoring sc("ACG", table.data(), below(6), below(4));
        const std::string a = random_string("ACG", 6), b = random_string("ACG", 6);
        const std::string c = random_string("ACG", 5), d = random_string("ACG", 5);
        agree += check_pair(a, b, sc, Mode::global, brute_global(a, b, sc))
                 && check_pair(c, d, sc, Mode::local, brute_local(c, d, sc));
    }
    std::printf("random pairs matching the brute force, global and local: %d of %d\n", agree, pairs);
}

void long_pairs(int pairs)
{
    const Scoring b62(blosum_letters, blosum62.data(), 11, 1);
    const std::string_view amino = blosum_letters.substr(0, 20);
    int agree = 0;
    for (int t = 0; t < pairs; ++t) {
        const std::string a = random_string(amino, 400), b = random_string(amino, 400);
        const int g = align(a, b, b62, Mode::global).score;
        const int l = align(a, b, b62, Mode::local).score;
        agree += check_pair(a, b, b62, Mode::global, g) && check_pair(a, b, b62, Mode::local, l)
                 && l >= g && l >= 0;
    }
    std::printf("protein pairs up to 400 letters where both functions agree: %d of %d\n", agree, pairs);
}

}  // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    known_results();
    against_brute_force(2000);
    long_pairs(100);
    std::printf("%d failed\n", failures);
    return failures != 0;
}
