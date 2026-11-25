"""Deliberate mistakes: put each one into a copy of the sources, build the
tests and the prediction program against it, and check that something fails.

Run from the example directory:  python3 tests/mutations.py [C++ compiler]
Every mistake must make test_tree fail or change predict_tree's output; if
one does not, the script exits with status 1. A mutant that hangs or crashes
counts as caught. Mutants are built with AddressSanitizer and
UndefinedBehaviorSanitizer, so out-of-range access fails loudly.
"""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import tempfile

EXAMPLE = pathlib.Path(__file__).resolve().parent.parent
HEADER = "src/tree.hpp"
GENERATED = "src/generated/breast_cancer_tree.hpp"

# (description, file, text, replacement)
MUTATIONS = [
    ("leaf() sends equal values right", HEADER,
     "            i = static_cast<std::size_t>(x[n.feature] <= n.threshold ? n.left : n.right);",
     "            i = static_cast<std::size_t>(x[n.feature] < n.threshold ? n.left : n.right);"),
    ("leaf() swaps the children", HEADER,
     "            i = static_cast<std::size_t>(x[n.feature] <= n.threshold ? n.left : n.right);",
     "            i = static_cast<std::size_t>(x[n.feature] <= n.threshold ? n.right : n.left);"),
    ("path() sends equal values right", HEADER,
     "            p.push_back(static_cast<std::size_t>(x[n.feature] <= n.threshold ? n.left : n.right));",
     "            p.push_back(static_cast<std::size_t>(x[n.feature] < n.threshold ? n.left : n.right));"),
    ("children before their parent accepted", HEADER,
     "                                   n.left <= id || n.right <= id || ", "                                   "),
    ("feature index not checked", HEADER,
     "(n.feature >= static_cast<int>(n_features) ||", "(false ||"),
    ("infinite threshold accepted", HEADER, " || !std::isfinite(n.threshold)))", "))"),
    ("trailing characters after a number accepted", HEADER,
     "ec != std::errc() || end != s.data() + s.size()", "ec != std::errc()"),
    ("node ids not checked", HEADER,
     "            if (id != static_cast<int>(t.nodes_.size()))", "            if (false)"),
    ("children past the last node accepted", HEADER,
     "                throw std::runtime_error(path + \": child index past the last node\");",
     "                (void)0;"),
    ("leaves() counts the splits", HEADER, "n += node.feature < 0;", "n += node.feature >= 0;"),
    ("benchmark order from another seed", HEADER, "SplitMix64 rng(2026);", "SplitMix64 rng(2025);"),
    ("generated code sends equal values right", GENERATED,
     "if (x[22] <= 114.44999999999999)", "if (x[22] < 114.44999999999999)"),
    ("generated threshold rounded to 6 digits", GENERATED,
     "if (x[27] <= 0.11099999999999999)", "if (x[27] <= 0.111)"),
]


def build(work: pathlib.Path, cxx: str, source: str, out: str) -> bool:
    flags = ["-std=c++17", "-O1", "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all"]
    result = subprocess.run([cxx, *flags, "-Isrc", source, "-o", out], cwd=work, capture_output=True)
    return result.returncode == 0


def fails(work: pathlib.Path, cxx: str) -> str:
    """Why the mutant was caught, or "" if everything still passed."""
    if not build(work, cxx, "tests/test_tree.cpp", "test_tree"):
        return "does not compile"
    if not build(work, cxx, "src/predict_tree.cpp", "predict_tree"):
        return "does not compile"
    tree, rows = str(work / "models" / "tree.txt"), str(work / "data" / "breast_cancer_test.csv")
    try:
        tests = subprocess.run(["./test_tree", tree, rows], cwd=work, capture_output=True, text=True, timeout=120)
        if tests.returncode != 0:
            failed = [line for line in tests.stdout.splitlines() if line.startswith("FAILED")]
            return f"test_tree: {len(failed)} check(s) failed" if failed else "test_tree crashed"
        out = subprocess.run(["./predict_tree", tree, rows], cwd=work, capture_output=True, text=True, timeout=60)
    except subprocess.TimeoutExpired:
        return "timed out"
    want = (work / "tests" / "expected" / "predict_exported.txt").read_text()
    if out.returncode != 0:
        return "predict_tree crashed"
    return "" if out.stdout == want else "predict_tree output changed"


def main() -> int:
    cxx = sys.argv[1] if len(sys.argv) > 1 else "g++"
    missed = 0
    print(f"{'mistake':<46} result")
    for name, file, old, new in MUTATIONS:
        with tempfile.TemporaryDirectory() as tmp:
            work = pathlib.Path(tmp)
            for sub in ("src", "tests", "data", "models"):
                shutil.copytree(EXAMPLE / sub, work / sub)
            target = work / file
            text = target.read_text()
            if text.count(old) != 1:
                print(f"{name}: pattern not found exactly once")
                return 2
            target.write_text(text.replace(old, new))
            why = fails(work, cxx)
            missed += not why
            print(f"{name:<46} {why or 'MISSED'}")
    print(f"\n{len(MUTATIONS) - missed} of {len(MUTATIONS)} mistakes caught")
    return 1 if missed else 0


if __name__ == "__main__":
    sys.exit(main())
