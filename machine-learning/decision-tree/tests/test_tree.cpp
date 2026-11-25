// test_tree.cpp - checks tree.hpp and the generated function: both agree with
// the Python model on the test rows, a value equal to a threshold goes left,
// malformed files are rejected, and the benchmark rows and checksum match the
// Python tests.
//
// Usage: test_tree <path to models/tree.txt> <path to data/breast_cancer_test.csv>
#include "../src/generated/breast_cancer_tree.hpp"
#include "../src/tree.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>

namespace {

int failures = 0;

void check(bool ok, const std::string& what, int line)
{
    if (!ok) {
        std::printf("FAILED line %d: %s\n", line, what.c_str());
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

template <class F>
std::string error_of(F f)
{
    try {
        f();
    } catch (const std::exception& e) {
        return e.what();
    }
    return "";
}

void write_file(const char* name, const char* text)
{
    std::ofstream out(name, std::ios::binary);
    out << text;
}

// Python's predictions for the 114 test rows (tests/expected/predict_exported.txt)
const char* const kPython =
    "010100100010100000100011100111101100001000100101001100010"
    "001001000110001100001011100001110100010000000010010100001";

void test_matches_python(const dtree::Tree& tree, const dtree::Rows& rows)
{
    CHECK(rows.size() == 114 && rows.names.size() == 30);
    CHECK(tree.nodes().size() == 13 && tree.leaves() == 7);
    int correct = 0;
    for (std::size_t r = 0; r < rows.size(); ++r) {
        const int p = tree.predict(rows.row(r));
        CHECK(p == kPython[r] - '0');
        CHECK(generated::breast_cancer_tree(rows.row(r)) == p);
        correct += p == rows.labels[r];
    }
    CHECK(correct == 107);
    const auto path = tree.path(rows.row(0));
    CHECK((path == std::vector<std::size_t>{0, 1, 2, 3}));
}

void test_generated_code_agrees_everywhere(const dtree::Tree& tree, const dtree::Rows& rows)
{
    // random rows near the thresholds: every feature value is a test value, a
    // threshold or the next double above one, so every comparison is exercised
    // exactly at its boundary on both sides
    std::vector<double> candidates;
    for (const dtree::Node& n : tree.nodes())
        if (n.feature >= 0) {
            candidates.push_back(n.threshold);
            candidates.push_back(std::nextafter(n.threshold, HUGE_VAL));
        }
    dtree::SplitMix64 rng(99);
    std::vector<double> x(30);
    int disagree = 0;
    for (int trial = 0; trial < 20000; ++trial) {
        for (std::size_t f = 0; f < x.size(); ++f) {
            const std::uint64_t pick = rng.next();
            x[f] = pick % 3 == 0 ? candidates[pick / 3 % candidates.size()]
                                 : rows.row(pick / 3 % rows.size())[f];
        }
        disagree += tree.predict(x.data()) != generated::breast_cancer_tree(x.data());
    }
    CHECK(disagree == 0);
}

void test_value_equal_to_threshold_goes_left(const dtree::Tree& tree)
{
    std::vector<double> x(30, 0.0);
    const dtree::Node& root = tree.nodes()[0];
    x[static_cast<std::size_t>(root.feature)] = root.threshold;
    CHECK(tree.path(x.data())[1] == static_cast<std::size_t>(root.left));
}

void test_rejects_bad_files()
{
    CHECK(error_of([] { (void)dtree::Tree::load("no_such_tree.txt", 1); }) ==
          "cannot open no_such_tree.txt");
    const struct {
        const char* text;
        const char* message;
    } bad[] = {
        {"0 0 1.5 1 2 0\n1 -1 0 -1 -1 0\n", "bad_tree.txt: child index past the last node"},
        {"0 0 1.5 1 2\n", "line 1: expected 6 fields"},
        {"0 0 1.5x 1 2 0\n", "line 1: bad threshold: '1.5x'"},
        {"1 -1 0 -1 -1 0\n", "line 1: node 1 out of order"},
        {"0 0 1.5 0 1 0\n1 -1 0 -1 -1 0\n", "line 1: invalid split"},
        {"0 5 1.5 1 2 0\n", "line 1: invalid split"},
        {"0 0 inf 1 2 0\n", "line 1: invalid split"},
        {"# nothing\n", "bad_tree.txt: no nodes"},
    };
    for (const auto& b : bad) {
        write_file("bad_tree.txt", b.text);
        const std::string got = error_of([] { (void)dtree::Tree::load("bad_tree.txt", 2); });
        CHECK(got == b.message);
        if (got != b.message)
            std::printf("  got: %s\n", got.c_str());
    }
    std::remove("bad_tree.txt");

    write_file("crlf.csv", "class,a,b\r\n1,0.5,2\r\n");
    const dtree::Rows rows = dtree::load_rows("crlf.csv");
    CHECK(rows.size() == 1 && rows.names.size() == 2 && rows.row(0)[1] == 2.0);
    write_file("crlf.csv", "class,a,b\n1,0.5\n");
    CHECK(error_of([] { (void)dtree::load_rows("crlf.csv"); }) == "line 2: expected 3 fields");
    std::remove("crlf.csv");
}

void test_benchmark_rows_match_python(const dtree::Tree& tree, const dtree::Rows& rows)
{
    dtree::SplitMix64 rng(0);
    CHECK(rng.next() == 0xE220A8397B1DCDAFULL);
    const std::vector<std::size_t> order = dtree::benchmark_order();
    CHECK(order.size() == 1000008);
    CHECK((std::vector<std::size_t>(order.begin(), order.begin() + 5) ==
           std::vector<std::size_t>{19, 83, 2, 30, 51}));
    std::int64_t sum = 0;
    for (std::size_t i = 0; i < order.size(); ++i)
        sum = (sum + static_cast<std::int64_t>(i + 1) * tree.predict(rows.row(order[i]))) %
              1000000007;
    CHECK(sum == 732426313);  // the same number test_tree.py checks
}

}  // namespace

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::printf("usage: test_tree <models/tree.txt> <data/breast_cancer_test.csv>\n");
        return 2;
    }
    try {
        const dtree::Rows rows = dtree::load_rows(argv[2]);
        const dtree::Tree tree = dtree::Tree::load(argv[1], rows.names.size());
        test_matches_python(tree, rows);
        test_generated_code_agrees_everywhere(tree, rows);
        test_value_equal_to_threshold_goes_left(tree);
        test_rejects_bad_files();
        test_benchmark_rows_match_python(tree, rows);
    } catch (const std::exception& e) {
        std::printf("FAILED: %s\n", e.what());
        return 1;
    }
    std::printf("%s\n", failures == 0 ? "all tests passed" : "some tests FAILED");
    return failures == 0 ? 0 : 1;
}
