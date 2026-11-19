# run_all.ps1 - the Windows checks behind the article's "What I Verified"
# table: MSVC builds with /W4 /WX /permissive- in C++17 and C++20, every CTest
# test (including the two programs that must not compile), deliberate
# mistakes that a test must catch, and one run of the benchmark.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
# Writes the result lines to results-windows.md; if something fails, the
# details are in the msvc-*.log files next to it.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $text) { $script:results += ("{0,-58} {1}" -f $name, $text) }
function Ok($b) { if ($b) { "PASS" } else { "FAIL" } }

# Configures, builds and tests one source tree. Returns "build-failed",
# "tests-failed" or "passed".
function Check($src, $dir, $std, $log) {
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    cmake -S $src -B $dir "-DCMAKE_CXX_STANDARD=$std" *> "$log-build.log"
    if ($LASTEXITCODE -eq 0) { cmake --build $dir --config Release *>> "$log-build.log" }
    if ($LASTEXITCODE -ne 0) { return "build-failed" }
    Remove-Item "$log-build.log"
    ctest --test-dir $dir -C Release --output-on-failure *> "$log-ctest.log"
    if ($LASTEXITCODE -ne 0) { return "tests-failed" }
    Remove-Item "$log-ctest.log"
    return "passed"
}

foreach ($std in 17, 20) {
    $r = Check "." "build-msvc-$std" $std "msvc-$std"
    Row "MSVC build C++$std (/W4 /WX /permissive-)" (Ok ($r -ne "build-failed"))
    if ($r -ne "build-failed") { Row "MSVC CTest C++$std (4 outputs, 2 compile failures)" (Ok ($r -eq "passed")) }
}

# Deliberate mistakes. Each one is applied to a copy of the code; a test must fail.
$mutants = @(
    @("src/typeid_basics.cpp", "virtual ~Shape() = default;", "~Shape() = default;", "Shape is no longer polymorphic"),
    @("src/dynamic_cast_basics.cpp", "} catch (const std::bad_cast&) {", "} catch (const std::bad_typeid&) {", "the wrong exception type caught"),
    @("src/alternatives.cpp", "if (auto* r = dynamic_cast<const Rect*>(&s))", "if (auto* r = dynamic_cast<const Rect*>(&s); r && r->w < 0)", "a missing branch in the dynamic_cast chain"),
    @("tests/compile_fail/not_polymorphic.cpp", "struct Point { int x = 0; };", "struct Point { int x = 0; virtual ~Point() = default; };", "compile-fail program made valid"),
    @("src/type_name.h", "name.erase(0, std::string(prefix).size());", "{}", "MSVC class/struct prefix kept")
)
# A mutant only counts as caught if the unchanged code passes first.
if ($results -cmatch " FAIL$") { Row "Mutants" "not run: the unchanged code fails its tests"; $mutants = @() }
$n = 0
foreach ($m in $mutants) {
    $n++
    $copy = Join-Path $env:TEMP "rtti-mutant-$n"
    if (Test-Path $copy) { Remove-Item -Recurse -Force $copy }
    New-Item -ItemType Directory $copy | Out-Null
    Copy-Item -Recurse src, tests, bench, CMakeLists.txt $copy
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
# The benchmark: informational only, never a pass or fail.
$bench = Join-Path "build-msvc-17" "Release\cast_cost.exe"
if (Test-Path $bench) { $report += "Benchmark (MSVC C++17 Release):"; $report += (& $bench) }
$report | Set-Content results-windows.md
$report
