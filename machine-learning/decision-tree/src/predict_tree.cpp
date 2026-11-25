// predict_tree.cpp - classify the 114 test tumours with the decision tree
// trained in Python. Prints exactly what the Python predict_exported.py prints;
// both are compared with the same tests/expected/predict_exported.txt.
//
// Usage: predict_tree [models/tree.txt] [data/breast_cancer_test.csv]
#include "tree.hpp"

#include <cstdio>
#include <exception>

int main(int argc, char** argv)
{
    try {
        const std::string tree_path = argc > 1 ? argv[1] : "models/tree.txt";
        const std::string csv_path = argc > 2 ? argv[2] : "data/breast_cancer_test.csv";
        const dtree::Rows rows = dtree::load_rows(csv_path);
        const dtree::Tree tree = dtree::Tree::load(tree_path, rows.names.size());

        std::printf("tree: %zu nodes, %zu leaves; test rows: %zu\n", tree.nodes().size(),
                    tree.leaves(), rows.size());
        int cm[2][2] = {{0, 0}, {0, 0}};
        std::string preds;
        for (std::size_t r = 0; r < rows.size(); ++r) {
            const int p = tree.predict(rows.row(r));
            ++cm[rows.labels[r]][p];
            preds += static_cast<char>('0' + p);
        }
        std::printf("correct: %d of %zu\n", cm[0][0] + cm[1][1], rows.size());
        std::printf("benign:    %3d right, %2d called malignant\n", cm[0][0], cm[0][1]);
        std::printf("malignant: %3d right, %2d missed\n", cm[1][1], cm[1][0]);
        std::printf("predictions (1 = malignant):\n");
        for (std::size_t start = 0; start < preds.size(); start += 57)
            std::printf("  %s\n", preds.substr(start, 57).c_str());

        const double* x = rows.row(0);
        const auto path = tree.path(x);
        std::printf("test row 0 (class %d) follows nodes", rows.labels[0]);
        for (std::size_t node : path)
            std::printf(" %zu", node);
        std::printf(":\n");
        for (std::size_t i = 0; i + 1 < path.size(); ++i) {
            const dtree::Node& n = tree.nodes()[path[i]];
            const double v = x[n.feature];
            std::printf("  %s = %.6g %s %.6g\n", rows.names[static_cast<std::size_t>(n.feature)].c_str(),
                        v, v <= n.threshold ? "<=" : ">", n.threshold);
        }
        std::printf("  leaf %zu: class %d\n", path.back(), tree.predict(x));
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "predict_tree: %s\n", e.what());
        return 1;
    }
}
