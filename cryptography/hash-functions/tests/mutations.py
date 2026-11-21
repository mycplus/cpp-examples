#!/usr/bin/env python3
"""Mutation check: break digest.hpp in one plausible way at a time and confirm that
test_vectors notices. A crash or a timeout counts as caught.

Usage: python3 tests/mutations.py        (run from the example directory; needs g++ or c++)
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC = open(os.path.join(ROOT, "include", "digest.hpp")).read()
CXX = shutil.which("g++") or shutil.which("c++")

MUTATIONS = [
    ("MD5: round 2 word index (5i+1 -> 5i+2)", "(5 * i + 1) & 15", "(5 * i + 2) & 15"),
    ("MD5: round 4 function (b | ~d -> b & ~d)", "c ^ (b | ~d)", "c ^ (b & ~d)"),
    ("MD5: one shift amount changed", "6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};",
     "6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 22};"),
    ("MD5: output words big-endian", "BlockHash<Md5, 16, false>", "BlockHash<Md5, 16, true>"),
    ("SHA-1: schedule rotation 1 -> 2", "w[t - 14] ^ w[t - 16], 1)", "w[t - 14] ^ w[t - 16], 2)"),
    ("SHA-1: majority function wrong", "(b & c) | (b & d) | (c & d)", "(b & c) | (b & d)"),
    ("SHA-1: initial value", "0xc3d2e1f0}", "0xc3d2e1f1}"),
    ("SHA-256: sigma0 rotation 7 -> 8", "detail::rotr(w[t - 15], 7)", "detail::rotr(w[t - 15], 8)"),
    ("SHA-256: Ch uses the wrong word", "(e & f) ^ (~e & g)", "(e & f) ^ (~e & f)"),
    ("SHA-256: last round constant", "0xc67178f2", "0xc67178f3"),
    ("SHA-256: Big Sigma1 rotation 25 -> 24", "detail::rotr(e, 25)", "detail::rotr(e, 24)"),
    ("padding: zero count off by one (55 -> 56)", "(55 - (bytes_ & 63)) & 63", "(56 - (bytes_ & 63)) & 63"),
    ("padding: length in bytes, not bits", "bytes_ * 8", "bytes_ * 1"),
    ("padding: 0x80 marker missing", "pad[0] = 0x80;", "pad[0] = 0x00;"),
    ("update: partial block never completed", "if (used + take < block_size)", "if (used + take <= block_size)"),
    ("update: byte count not added", "bytes_ += len;", ""),
    ("HMAC: inner pad 0x36 -> 0x35", "k[i] ^ 0x36", "k[i] ^ 0x35"),
    ("HMAC: outer pad 0x5c -> 0x5d", "k[i] ^ 0x5c", "k[i] ^ 0x5d"),
    ("HMAC: long keys not hashed first", "if (key.size() > k.size())", "if (key.size() > 400)"),
]

caught = 0
with tempfile.TemporaryDirectory() as tmp:
    for label, old, new in MUTATIONS:
        if SRC.count(old) != 1:
            sys.exit(f"mutation does not apply exactly once ({SRC.count(old)}): {label}")
        os.makedirs(os.path.join(tmp, "include"), exist_ok=True)
        with open(os.path.join(tmp, "include", "digest.hpp"), "w") as f:
            f.write(SRC.replace(old, new))
        exe = os.path.join(tmp, "t")
        build = subprocess.run([CXX, "-std=c++17", "-O1", "-w", "-I" + os.path.join(tmp, "include"),
                                os.path.join(HERE, "test_vectors.cpp"), "-o", exe],
                               capture_output=True, text=True)
        if build.returncode != 0:
            sys.exit(f"mutant does not compile: {label}\n{build.stderr}")
        try:
            run = subprocess.run([exe, os.path.join(HERE, "vectors")], capture_output=True, timeout=60)
            status = "caught" if run.returncode != 0 else "MISSED"
        except subprocess.TimeoutExpired:
            status = "caught"
        if status == "caught":
            caught += 1
        print(f"{status:7} {label}")
print(f"{caught} of {len(MUTATIONS)} mutations caught")
sys.exit(0 if caught == len(MUTATIONS) else 1)
