# run_all.ps1 - the Windows checks behind the article's "What I Verified"
# table: MSVC builds with /W4 /WX /permissive- in C++17 and C++20, every CTest
# test, and deliberate mistakes that a test must catch.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
# Writes the result lines to results-windows.md; if something fails, the
# details are in the msvc-*.log files next to it.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $text) { $script:results += ("{0,-58} {1}" -f $name, $text) }
function Ok($b) { if ($b) { "PASS" } else { "FAIL" } }

# Configures, builds and tests one source tree. Returns "build-failed",
# "tests-failed" or "passed". A hang counts as a failure after 30 seconds.
function Check($src, $dir, $std, $log) {
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    cmake -S $src -B $dir "-DCMAKE_CXX_STANDARD=$std" *> "$log-build.log"
    if ($LASTEXITCODE -eq 0) { cmake --build $dir --config Release *>> "$log-build.log" }
    if ($LASTEXITCODE -ne 0) { return "build-failed" }
    Remove-Item "$log-build.log"
    ctest --test-dir $dir -C Release --timeout 30 --output-on-failure *> "$log-ctest.log"
    if ($LASTEXITCODE -ne 0) { return "tests-failed" }
    Remove-Item "$log-ctest.log"
    return "passed"
}

foreach ($std in 17, 20) {
    $r = Check "." "build-msvc-$std" $std "msvc-$std"
    Row "MSVC build C++$std (/W4 /WX /permissive-)" (Ok ($r -ne "build-failed"))
    if ($r -ne "build-failed") { Row "MSVC CTest C++$std (7 tests)" (Ok ($r -eq "passed")) }
}

# Repeat the suite: a concurrency test that passes once can still be flaky.
if (Test-Path "build-msvc-17") {
    ctest --test-dir build-msvc-17 -C Release --timeout 30 --repeat until-fail:20 *> "msvc-repeat.log"
    Row "MSVC CTest C++17, every test 20 times" (Ok ($LASTEXITCODE -eq 0))
    if ($LASTEXITCODE -eq 0) { Remove-Item "msvc-repeat.log" }
}

# Deliberate mistakes. Each one is applied to a copy of the code; a test must fail.
# (The removed-lock mutant needs ThreadSanitizer, which MSVC does not have; it runs on Linux.)
$mutants = @(
    @("src/bounded_queue.h", "not_empty_.notify_all();", "/* no wake-up */", "close() does not wake the consumers"),
    @("src/transfers.cpp", "std::scoped_lock lock(from.m, to.m);", "std::lock_guard<std::mutex> l1(from.m); std::lock_guard<std::mutex> l2(to.m);", "locks taken one at a time (deadlock)"),
    @("src/cpp20_sync.cpp", "std::counting_semaphore<2> slots(2);", "std::counting_semaphore<3> slots(3);", "semaphore allows 3 threads")
)
# A mutant only counts as caught if the unchanged code passes first.
if ($results -cmatch " FAIL$") { Row "Mutants" "not run: the unchanged code fails its tests"; $mutants = @() }
$n = 0
foreach ($m in $mutants) {
    $n++
    $copy = Join-Path $env:TEMP "concurrency-mutant-$n"
    if (Test-Path $copy) { Remove-Item -Recurse -Force $copy }
    New-Item -ItemType Directory $copy | Out-Null
    Copy-Item -Recurse src, tests, CMakeLists.txt $copy
    $file = Join-Path $copy $m[0]
    $text = [System.IO.File]::ReadAllText($file)
    if (-not $text.Contains($m[1])) { Row "Mutant ${n}: $($m[3])" "NOT APPLIED"; continue }
    [System.IO.File]::WriteAllText($file, $text.Replace($m[1], $m[2]))
    $r = Check $copy (Join-Path $copy "build") 17 "msvc-mutant-$n"
    $verdict = switch ($r) { "tests-failed" { "CAUGHT" } "passed" { "MISSED" } default { "BUILD FAILED" } }
    Remove-Item -ErrorAction SilentlyContinue "msvc-mutant-$n-ctest.log"
    Row "Mutant ${n}: $($m[3])" $verdict
    Remove-Item -Recurse -Force $copy
}

$report = @("Environment: " + [System.Environment]::OSVersion.VersionString)
$cl = Get-Command cl.exe -ErrorAction SilentlyContinue
if ($cl) { $report += "cl.exe " + $cl.Version }
$report += $results
# What the unsynchronized counter printed on this machine (informational).
$counter = Join-Path "build-msvc-17" "Release\counter.exe"
if (Test-Path $counter) { $report += "counter racy (Release, 3 runs):"; 1..3 | ForEach-Object { $report += (& $counter racy) } }
$report | Set-Content results-windows.md
$report
