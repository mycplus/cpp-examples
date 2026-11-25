// knn_wine.cpp - k-nearest neighbors on the wine data, unscaled and
// standardized. Prints exactly what the Python version (wine_knn.py) prints;
// both are compared with the same tests/expected/wine_knn.txt.
//
// Usage: knn_wine [path to wine_split.csv]
#include "knn.hpp"

#include <cstdio>
#include <exception>

namespace {

// Share of all squared differences between training rows that the last
// column (proline) supplies.
double last_column_share(const knn::Dataset& d)
{
    double last = 0.0, total = 0.0;
    for (std::size_t i = 0; i < d.rows; ++i)
        for (std::size_t j = 0; j < d.rows; ++j)
            for (std::size_t c = 0; c < d.cols; ++c) {
                const double diff = d.row(i)[c] - d.row(j)[c];
                total += diff * diff;
                if (c + 1 == d.cols)
                    last += diff * diff;
            }
    return last / total;
}

int correct(const knn::Dataset& train, const knn::Dataset& test, std::size_t k, knn::Metric m)
{
    const std::vector<int> pred = knn::predict(train, test, k, m);
    int n = 0;
    for (std::size_t r = 0; r < test.rows; ++r)
        n += pred[r] == test.labels[r];
    return n;
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        const std::string path = argc > 1 ? argv[1] : "data/wine_split.csv";
        auto [raw_train, raw_test] = knn::load_split_csv(path);
        knn::Dataset train = raw_train, test = raw_test;
        knn::standardize(train, test);

        std::printf("wine: %zu training rows, %zu test rows, %zu features\n", train.rows,
                    test.rows, train.cols);
        std::printf("share of squared distance from proline, unscaled: %.1f%%\n",
                    100.0 * last_column_share(raw_train));
        std::printf("share of squared distance from proline, standardized: %.1f%%\n",
                    100.0 * last_column_share(train));

        std::printf("\ncorrect test predictions (of %zu)\n", test.rows);
        std::printf(" k  metric     unscaled  standardized\n");
        const struct {
            std::size_t k;
            knn::Metric metric;
            const char* name;
        } runs[] = {{1, knn::Metric::euclidean, "euclidean"},
                    {5, knn::Metric::euclidean, "euclidean"},
                    {27, knn::Metric::euclidean, "euclidean"},
                    {7, knn::Metric::manhattan, "manhattan"}};
        for (const auto& run : runs)
            std::printf("%2zu  %-9s  %8d  %12d\n", run.k, run.name,
                        correct(raw_train, raw_test, run.k, run.metric),
                        correct(train, test, run.k, run.metric));

        std::printf("\nnearest training rows to test row 0 (class %d), standardized\n",
                    test.labels[0]);
        std::printf("rank  row  class  distance\n");
        const auto nbrs = knn::nearest(train, test.row(0), 5);
        for (std::size_t i = 0; i < nbrs.size(); ++i)
            std::printf("%4zu  %3zu  %5d  %8.6f\n", i + 1, nbrs[i].index,
                        train.labels[nbrs[i].index], nbrs[i].distance);
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "knn_wine: %s\n", e.what());
        return 1;
    }
}
