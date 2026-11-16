// align.cpp - optimal pairwise alignment in C++17: global (Needleman-Wunsch)
// and local (Smith-Waterman) with affine gap costs (Gotoh's three tables).
// Build: g++ -std=c++17 -Wall -Wextra -pedantic align.cpp -o align
#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "blosum.hpp"

// How an alignment is scored. A gap of length k costs open + k * extend.
class Scoring {
public:
    Scoring(std::string_view letters, const int* table, int open, int extend)
        : size_(letters.size()), table_(table, table + letters.size() * letters.size()),
          open_(open), extend_(extend)
    {
        if (letters.empty() || open < 0 || extend < 0 || open > limit || extend > limit)
            throw std::invalid_argument("empty alphabet or gap cost out of range");
        step_ = open + extend;
        for (const int v : table_) {
            if (v > limit || v < -limit)
                throw std::invalid_argument("table score out of range");
            step_ = std::max(step_, v < 0 ? -v : v);
        }
        index_.fill(-1);
        for (std::size_t k = 0; k < letters.size(); ++k) {
            const auto c = static_cast<unsigned char>(letters[k]);
            index_[c] = static_cast<int>(k);
            if (c >= 'A' && c <= 'Z')
                index_[c - 'A' + 'a'] = static_cast<int>(k);   // lowercase matches too
        }
    }

    // The table row of a letter; throws for a letter outside the alphabet.
    std::size_t code(char c) const
    {
        const int k = index_[static_cast<unsigned char>(c)];
        if (k < 0)
            throw std::invalid_argument(std::string("letter outside the alphabet: ") + c);
        return static_cast<std::size_t>(k);
    }
    int score(std::size_t x, std::size_t y) const { return table_[x * size_ + y]; }
    int open() const { return open_; }
    // The most one column can change a score; n + m columns must not
    // overflow an int.
    void check_lengths(std::size_t n, std::size_t m) const
    {
        const std::size_t most = static_cast<std::size_t>(INT_MAX / 8 / std::max(step_, 1));
        if (n > most || m > most - n)
            throw std::length_error("sequences too long for int scores");
    }
    int extend() const { return extend_; }

private:
    static constexpr int limit = 1 << 20;
    std::array<int, 256> index_{};
    std::size_t size_;
    std::vector<int> table_;
    int open_, extend_, step_ = 0;
};

enum class Mode { global, local };

struct Alignment {
    std::string top, bottom;   // same length, '-' marks a gap
    int score = 0;
    std::size_t start1 = 0, end1 = 0, start2 = 0, end2 = 0;   // aligned parts: [start, end)
};

namespace detail {

constexpr int neg = INT_MIN / 4;   // "minus infinity" with room to subtract

// Best score of s1[0, i) against s2[0, j) ending in a pair (m), in s1[i-1]
// against a gap (x), or in s2[j-1] against a gap (y).
struct Cell {
    int m, x, y;
};

std::vector<std::size_t> encode(std::string_view s, const Scoring& sc)
{
    std::vector<std::size_t> codes(s.size());
    std::transform(s.begin(), s.end(), codes.begin(), [&](char c) { return sc.code(c); });
    return codes;
}

}  // namespace detail

Alignment align(std::string_view s1, std::string_view s2, const Scoring& sc, Mode mode)
{
    using detail::Cell;
    using detail::neg;
    const auto a = detail::encode(s1, sc), b = detail::encode(s2, sc);
    const std::size_t n = a.size(), m = b.size();
    sc.check_lengths(n, m);
    const bool local = mode == Mode::local;
    const int open = sc.open() + sc.extend(), ext = sc.extend();

    std::vector<Cell> t((n + 1) * (m + 1));
    auto at = [&](std::size_t i, std::size_t j) -> Cell& { return t[i * (m + 1) + j]; };
    for (std::size_t i = 0; i <= n; ++i) {
        for (std::size_t j = 0; j <= m; ++j) {
            Cell& c = at(i, j);
            if (i == 0 || j == 0) {
                c.m = (local || (i == 0 && j == 0)) ? 0 : neg;
                c.x = (!local && j == 0 && i > 0) ? -(sc.open() + static_cast<int>(i) * ext) : neg;
                c.y = (!local && i == 0 && j > 0) ? -(sc.open() + static_cast<int>(j) * ext) : neg;
                continue;
            }
            const Cell& d = at(i - 1, j - 1);
            const Cell& up = at(i - 1, j);
            const Cell& left = at(i, j - 1);
            int diag = std::max({d.m, d.x, d.y});
            if (local && diag < 0)
                diag = 0;                      // start a new local alignment here
            c.m = sc.score(a[i - 1], b[j - 1]) + diag;
            c.x = std::max({up.m - open, up.x - ext, up.y - open});
            c.y = std::max({left.m - open, left.y - ext, left.x - open});
        }
    }

    // Where the walk back starts: the best M cell for a local alignment
    // (or nowhere, if nothing scores above 0), the corner for a global one.
    Alignment al;
    std::size_t i = n, j = m;
    char state = 'm';
    if (local) {
        i = j = 0;
        for (std::size_t r = 1; r <= n; ++r)
            for (std::size_t c = 1; c <= m; ++c)
                if (at(r, c).m > al.score) {
                    al.score = at(r, c).m;
                    i = r;
                    j = c;
                }
    } else {
        const Cell& c = at(n, m);
        al.score = std::max({c.m, c.x, c.y});
        state = al.score == c.m ? 'm' : al.score == c.x ? 'x' : 'y';
    }
    al.end1 = i;
    al.end2 = j;
    while (i > 0 || j > 0) {
        if (state == 'm') {
            const int v = at(i, j).m - sc.score(a[i - 1], b[j - 1]);
            al.top += s1[--i];
            al.bottom += s2[--j];
            if (local && v == 0)
                break;                         // the alignment starts at this pair
            const Cell& p = at(i, j);
            state = v == p.m ? 'm' : v == p.x ? 'x' : 'y';
        } else if (state == 'x') {
            const int v = at(i, j).x;
            al.top += s1[--i];
            al.bottom += '-';
            const Cell& p = at(i, j);
            state = v == p.m - open ? 'm' : v == p.x - ext ? 'x' : 'y';
        } else {
            const int v = at(i, j).y;
            al.top += '-';
            al.bottom += s2[--j];
            const Cell& p = at(i, j);
            state = v == p.m - open ? 'm' : v == p.y - ext ? 'y' : 'x';
        }
    }
    al.start1 = i;
    al.start2 = j;
    std::reverse(al.top.begin(), al.top.end());       // the walk ran backwards
    std::reverse(al.bottom.begin(), al.bottom.end());
    return al;
}

// The optimal score only, keeping one row of cells instead of the table.
int align_score(std::string_view s1, std::string_view s2, const Scoring& sc, Mode mode)
{
    using detail::Cell;
    using detail::neg;
    const auto a = detail::encode(s1, sc), b = detail::encode(s2, sc);
    const std::size_t m = b.size();
    sc.check_lengths(a.size(), m);
    const bool local = mode == Mode::local;
    const int open = sc.open() + sc.extend(), ext = sc.extend();
    std::vector<Cell> prev(m + 1), cur(m + 1);
    for (std::size_t j = 0; j <= m; ++j)
        prev[j] = {(local || j == 0) ? 0 : neg, neg,
                   (!local && j > 0) ? -(sc.open() + static_cast<int>(j) * ext) : neg};
    int best = 0;
    for (std::size_t i = 1; i <= a.size(); ++i) {
        cur[0] = {local ? 0 : neg, local ? neg : -(sc.open() + static_cast<int>(i) * ext), neg};
        for (std::size_t j = 1; j <= m; ++j) {
            int diag = std::max({prev[j - 1].m, prev[j - 1].x, prev[j - 1].y});
            if (local && diag < 0)
                diag = 0;
            cur[j].m = sc.score(a[i - 1], b[j - 1]) + diag;
            cur[j].x = std::max({prev[j].m - open, prev[j].x - ext, prev[j].y - open});
            cur[j].y = std::max({cur[j - 1].m - open, cur[j - 1].y - ext, cur[j - 1].x - open});
            best = std::max(best, cur[j].m);
        }
        std::swap(prev, cur);
    }
    return local ? best : std::max({prev[m].m, prev[m].x, prev[m].y});
}

// The two rows with '|' under identical letters and '+' under other pairs
// that score above 0, in blocks of 60 columns.
void print(std::ostream& out, const Alignment& al, const Scoring& sc)
{
    for (std::size_t from = 0; from < al.top.size(); from += 60) {
        const std::size_t len = std::min<std::size_t>(60, al.top.size() - from);
        std::string marks;
        for (std::size_t k = from; k < from + len; ++k) {
            const char x = al.top[k], y = al.bottom[k];
            if (x == '-' || y == '-')
                marks += ' ';
            else if (sc.code(x) == sc.code(y))
                marks += '|';
            else
                marks += sc.score(sc.code(x), sc.code(y)) > 0 ? '+' : ' ';
        }
        if (from > 0)
            out << '\n';
        out << al.top.substr(from, len) << '\n' << marks << '\n'
            << al.bottom.substr(from, len) << '\n';
    }
}

namespace {

constexpr std::array<int, 16> dna = {
     3, -3, -3, -3,
    -3,  3, -3, -3,
    -3, -3,  3, -3,
    -3, -3, -3,  3,
};

void show(const char* title, std::string_view s1, std::string_view s2, const Scoring& sc, Mode mode)
{
    const Alignment al = align(s1, s2, sc, mode);
    std::cout << title << '\n';
    print(std::cout, al, sc);
    std::cout << "score " << al.score;
    if (mode == Mode::local)
        std::cout << ", letters " << al.start1 + 1 << '-' << al.end1 << " of the first and "
                  << al.start2 + 1 << '-' << al.end2 << " of the second";
    std::cout << "\n\n";
}

}  // namespace

int main()
{
    try {
        const Scoring dna_linear("ACGT", dna.data(), 0, 2);   // every gap position costs 2
        const Scoring dna_affine("ACGT", dna.data(), 8, 1);   // a gap of k costs 8 + k
        const Scoring b50(blosum_letters, blosum50.data(), 0, 8);

        show("Local, DNA, linear gaps", "TGTTACGG", "GGTTGACTA", dna_linear, Mode::local);
        show("Global, DNA, linear gaps", "TGTTACGG", "GGTTGACTA", dna_linear, Mode::global);
        show("Global, BLOSUM50, linear gaps", "HEAGAWGHEE", "PAWHEAE", b50, Mode::global);
        show("Local, BLOSUM50, linear gaps", "HEAGAWGHEE", "PAWHEAE", b50, Mode::local);
        show("Global, DNA, linear gaps", "GAAAAAGATAGAGC", "GAAAAAGAAAC", dna_linear, Mode::global);
        show("Global, DNA, affine gaps", "GAAAAAGATAGAGC", "GAAAAAGAAAC", dna_affine, Mode::global);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
