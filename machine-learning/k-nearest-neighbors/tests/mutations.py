"""Deliberate mistakes: put each one into a copy of knn.hpp, build the tests
and the wine program against it, and check that something fails.

Run from the example directory:  python3 tests/mutations.py [C++ compiler]
Every mistake must make test_knn fail or change the wine program's output;
if one does not, the script exits with status 1. A mutant that hangs or
crashes counts as caught. Mutants are built with AddressSanitizer and
UndefinedBehaviorSanitizer, so out-of-range access fails loudly.
"""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import tempfile

EXAMPLE = pathlib.Path(__file__).resolve().parent.parent

# (description, text in src/knn.hpp, replacement)
MUTATIONS = [
    ("square root left out of the distance", "return std::sqrt(sum);", "return sum;"),
    ("manhattan distance without fabs", "sum += std::fabs(a[i] - b[i]);", "sum += a[i] - b[i];"),
    (
        "equal distances not ordered by index",
        " || (a.distance == b.distance && a.index < b.index)",
        "",
    ),
    ("nth_element finds only the nearest", "kth - 1, all.end()", "all.begin(), all.end()"),
    ("the k nearest never sorted", "    std::sort(all.begin(), kth, closer);\n", ""),
    ("k larger than the data accepted", "k == 0 || k > train.rows", "k == 0"),
    ("tied vote goes to the larger label", "if (v.second > best.second)", "if (v.second >= best.second)"),
    (
        "column count not checked",
        "    if (queries.cols != train.cols)\n        throw",
        "    if (false)\n        throw",
    ),
    ("mean not divided by the row count", "        mean /= static_cast<double>(train.rows);\n", ""),
    (
        "sample standard deviation (n - 1)",
        "std::sqrt(var / static_cast<double>(train.rows))",
        "std::sqrt(var / static_cast<double>(train.rows - 1))",
    ),
    (
        "trailing characters after a number accepted",
        "ec != std::errc() || end != s.data() + s.size()",
        "ec != std::errc()",
    ),
    (
        "Windows line endings not stripped",
        "    for (std::size_t line = 2; std::getline(in, text); ++line) {\n"
        "        if (!text.empty() && text.back() == '\\r')\n            text.pop_back();\n",
        "    for (std::size_t line = 2; std::getline(in, text); ++line) {\n",
    ),
    ("splitmix64 increment wrong", "0x9E3779B97F4A7C15ULL", "0x9E3779B97F4A7C17ULL"),
    ("uniform from 52 bits", "(next() >> 11) * 0x1.0p-53", "(next() >> 12) * 0x1.0p-53"),
]


def build(work: pathlib.Path, cxx: str, source: str, out: str) -> bool:
    flags = ["-std=c++17", "-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all"]
    result = subprocess.run([cxx, *flags, source, "-o", out], cwd=work, capture_output=True)
    return result.returncode == 0


def fails(work: pathlib.Path, cxx: str) -> str:
    """Why the mutant was caught, or "" if everything still passed."""
    if not build(work, cxx, "tests/test_knn.cpp", "test_knn"):
        return "does not compile"
    if not build(work, cxx, "src/knn_wine.cpp", "knn_wine"):
        return "does not compile"
    csv = str(work / "data" / "wine_split.csv")
    try:
        tests = subprocess.run(["./test_knn", csv], cwd=work, capture_output=True, text=True, timeout=60)
        if tests.returncode != 0:
            failed = [line for line in tests.stdout.splitlines() if line.startswith("FAILED")]
            return f"test_knn: {len(failed)} check(s) failed" if failed else "test_knn crashed"
        wine = subprocess.run(["./knn_wine", csv], cwd=work, capture_output=True, text=True, timeout=60)
    except subprocess.TimeoutExpired:
        return "timed out"
    want = (work / "tests" / "expected" / "wine_knn.txt").read_text()
    if wine.returncode != 0:
        return "knn_wine crashed"
    return "" if wine.stdout == want else "knn_wine output changed"


def main() -> int:
    cxx = sys.argv[1] if len(sys.argv) > 1 else "g++"
    missed = 0
    print(f"{'mistake':<46} result")
    for name, old, new in MUTATIONS:
        with tempfile.TemporaryDirectory() as tmp:
            work = pathlib.Path(tmp)
            for sub in ("src", "tests", "data"):
                shutil.copytree(EXAMPLE / sub, work / sub)
            header = work / "src" / "knn.hpp"
            text = header.read_text()
            if text.count(old) != 1:
                print(f"{name}: pattern not found exactly once")
                return 2
            header.write_text(text.replace(old, new))
            why = fails(work, cxx)
            missed += not why
            print(f"{name:<46} {why or 'MISSED'}")
    print(f"\n{len(MUTATIONS) - missed} of {len(MUTATIONS)} mistakes caught")
    return 1 if missed else 0


if __name__ == "__main__":
    sys.exit(main())
