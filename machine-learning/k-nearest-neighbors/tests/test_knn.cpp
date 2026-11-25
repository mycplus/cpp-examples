// test_knn.cpp - checks knn.hpp: the article's worked example, tie rules,
// nearest() against a full sort, standardization, the CSV reader, and the
// splitmix64 data shared with the Python tests.
//
// Usage: test_knn <path to data/wine_split.csv>
#include "../src/knn.hpp"

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

bool near_equal(double a, double b, double tol = 1e-12)
{
    return std::fabs(a - b) <= tol * (1.0 + std::fabs(b));
}

knn::Dataset make(std::size_t cols, const std::vector<double>& values,
                  const std::vector<int>& labels)
{
    knn::Dataset d;
    d.cols = cols;
    d.rows = labels.size();
    d.values = values;
    d.labels = labels;
    return d;
}

template <class F>
bool throws(F f)
{
    try {
        f();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}

// The article's seven points; A = 0, B = 1; query (3, 3).
void test_worked_example()
{
    const knn::Dataset pts =
        make(2, {2, 4, 0, 4, 5, 4, 0, 1, 3, 6, 2, 3, 5, 5}, {0, 0, 0, 0, 1, 1, 1});
    const double q[] = {3.0, 3.0};
    const int want[] = {1, 0, 1, 0};  // k = 1, 3, 5, 7: B, A, B, A
    const std::size_t ks[] = {1, 3, 5, 7};
    for (int i = 0; i < 4; ++i)
        CHECK(knn::predict_one(pts, q, ks[i]) == want[i]);

    const auto nbrs = knn::nearest(pts, q, 7);
    const std::size_t order[] = {5, 0, 2, 6, 4, 1, 3};
    const double squared[] = {1, 2, 5, 8, 9, 10, 13};
    for (std::size_t i = 0; i < 7; ++i) {
        CHECK(nbrs[i].index == order[i]);
        CHECK(near_equal(nbrs[i].distance * nbrs[i].distance, squared[i]));
    }
    const auto man = knn::nearest(pts, q, 7, knn::Metric::manhattan);
    CHECK(man[0].index == 5 && near_equal(man[0].distance, 1.0));
    CHECK(man[6].index == 3 && near_equal(man[6].distance, 5.0));
}

void test_ties()
{
    // points at distance 1 on both sides: the lower index comes first
    const knn::Dataset line = make(1, {4.0, 1.0, 0.0, 3.0, 2.0}, {0, 1, 2, 3, 4});
    const double q[] = {2.0};
    const auto nbrs = knn::nearest(line, q, 4);
    CHECK(nbrs[0].index == 4);
    CHECK(nbrs[1].index == 1 && nbrs[2].index == 3);
    CHECK(nbrs[3].index == 0);

    // one vote each: the smaller label wins, whatever the order
    const double mid[] = {1.0};
    CHECK(knn::predict_one(make(1, {0.0, 2.0}, {5, 3}), mid, 2) == 3);
    CHECK(knn::predict_one(make(1, {0.0, 2.0}, {3, 5}), mid, 2) == 3);
}

void test_nearest_matches_a_full_sort()
{
    knn::SplitMix64 rng(7);
    for (int trial = 0; trial < 50; ++trial) {
        const std::size_t rows = 1 + rng.next() % 300, cols = 1 + rng.next() % 6;
        knn::Dataset d;
        d.rows = rows;
        d.cols = cols;
        for (std::size_t i = 0; i < rows * cols; ++i)
            d.values.push_back(static_cast<double>(rng.next() % 5));  // many equal distances
        d.labels.assign(rows, 0);
        std::vector<double> q(cols);
        for (double& v : q)
            v = static_cast<double>(rng.next() % 5);
        for (knn::Metric m : {knn::Metric::euclidean, knn::Metric::manhattan}) {
            std::vector<knn::Neighbor> all;
            for (std::size_t r = 0; r < rows; ++r)
                all.push_back({knn::distance(q.data(), d.row(r), cols, m), r});
            std::stable_sort(all.begin(), all.end(), [](const auto& a, const auto& b) {
                return a.distance < b.distance;
            });
            const std::size_t k = 1 + rng.next() % rows;
            const auto nbrs = knn::nearest(d, q.data(), k, m);
            bool same = nbrs.size() == k;
            for (std::size_t i = 0; same && i < k; ++i)
                same = nbrs[i].index == all[i].index && nbrs[i].distance == all[i].distance;
            CHECK(same);
        }
    }
}

void test_bad_k()
{
    const knn::Dataset d = make(1, {0.0, 1.0}, {0, 1});
    const double q[] = {0.5};
    CHECK(throws([&] { (void)knn::nearest(d, q, 0); }));
    CHECK(throws([&] { (void)knn::nearest(d, q, 3); }));
    const knn::Dataset wide = make(2, {0.0, 0.0}, {0});
    CHECK(throws([&] { (void)knn::predict(d, wide, 1); }));
}

void test_standardize()
{
    knn::Dataset train = make(2, {1, 10, 2, 20, 3, 30, 6, 60}, {0, 0, 1, 1});
    knn::Dataset test = make(2, {3, 30}, {0});
    knn::standardize(train, test);
    for (std::size_t c = 0; c < 2; ++c) {
        double mean = 0, sq = 0;
        for (std::size_t r = 0; r < 4; ++r)
            mean += train.row(r)[c] / 4;
        for (std::size_t r = 0; r < 4; ++r)
            sq += (train.row(r)[c] - mean) * (train.row(r)[c] - mean) / 4;
        CHECK(near_equal(mean + 1.0, 1.0));
        CHECK(near_equal(sq, 1.0));
    }
    // the test row is scaled with the training mean (3, 30), so it maps to 0
    CHECK(near_equal(test.row(0)[0] + 1.0, 1.0) && near_equal(test.row(0)[1] + 1.0, 1.0));
    knn::Dataset flat = make(1, {2, 2}, {0, 1}), none = make(1, {}, {});
    CHECK(throws([&] { knn::standardize(flat, none); }));
}

void test_csv(const std::string& path)
{
    const auto [train, test] = knn::load_split_csv(path);
    CHECK(train.rows == 133 && test.rows == 45 && train.cols == 13);
    CHECK(train.values.size() == 133 * 13 && test.labels.size() == 45);
    CHECK(train.row(0)[0] == 14.83 && train.row(0)[12] == 1045.0 && train.labels[0] == 0);

    CHECK(throws([] { (void)knn::load_split_csv("no_such_file.csv"); }));
    {
        std::ofstream out("crlf.csv", std::ios::binary);  // Windows line endings
        out << "split,class,a\r\ntrain,0,1.5\r\ntest,1,2\r\n";
    }
    const auto [crlf_train, crlf_test] = knn::load_split_csv("crlf.csv");
    CHECK(crlf_train.rows == 1 && crlf_train.values[0] == 1.5 && crlf_test.labels[0] == 1);
    std::remove("crlf.csv");
    const struct {
        const char* name;
        const char* text;
        const char* message;
    } bad[] = {
        {"bad_number.csv", "split,class,a\ntrain,0,1.5x\n", "line 2: not a number: '1.5x'"},
        {"bad_fields.csv", "split,class,a\ntrain,0\n", "line 2: expected 3 fields"},
        {"bad_split.csv", "split,class,a\nvalid,0,1\n", "line 2: split must be train or test"},
    };
    for (const auto& b : bad) {
        {
            std::ofstream out(b.name);
            out << b.text;
        }
        std::string got;
        try {
            (void)knn::load_split_csv(b.name);
        } catch (const std::exception& e) {
            got = e.what();
        }
        CHECK(got == b.message);
        std::remove(b.name);
    }
}

void test_splitmix_and_synthetic_match_python()
{
    knn::SplitMix64 rng(0);
    CHECK(rng.next() == 0xE220A8397B1DCDAFULL);
    CHECK(rng.next() == 0x6E789E6AA1B965F4ULL);
    // the same three numbers are checked in the Python tests (test_knn.py)
    const knn::Dataset train = knn::synthetic(2000, 8, 1);
    const knn::Dataset queries = knn::synthetic(200, 8, 2);
    CHECK(train.row(0)[0] == 0.5665615751722809);
    int label_sum = 0;
    for (int l : train.labels)
        label_sum += l;
    CHECK(label_sum == 1972);
    const std::vector<int> pred = knn::predict(train, queries, 5);
    long long checksum = 0;
    for (std::size_t i = 0; i < pred.size(); ++i)
        checksum += static_cast<long long>(i + 1) * pred[i];
    CHECK(checksum == 20892);
}

}  // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::printf("usage: test_knn <path to wine_split.csv>\n");
        return 2;
    }
    test_worked_example();
    test_ties();
    test_nearest_matches_a_full_sort();
    test_bad_k();
    test_standardize();
    test_csv(argv[1]);
    test_splitmix_and_synthetic_match_python();
    std::printf("%s\n", failures == 0 ? "all tests passed" : "some tests FAILED");
    return failures == 0 ? 0 : 1;
}
