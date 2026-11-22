# run_all.ps1 - the Windows checks for the C++ version.
#   1. Builds with MSVC (x64, C++17 and C++20, /W4 /WX) and runs CTest,
#      including the [[nodiscard]] compile-fail test, which calls cl directly.
#   2. Applies deliberate mistakes that a test must catch.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
Set-Location $PSScriptRoot
$results = @()
function Row($name, $text) { $script:results += ("{0,-58} {1}" -f $name, $text) }

function Check($src, $dir, $extra, $log) {
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    cmake -S $src -B $dir -A x64 @extra *> "$log-build.log"
    if ($LASTEXITCODE -ne 0) { return "configure-failed" }
    cmake --build $dir --config Release *>> "$log-build.log"
    if ($LASTEXITCODE -ne 0) { return "build-failed" }
    Remove-Item "$log-build.log"
    ctest --test-dir $dir -C Release --output-on-failure *> "$log-ctest.log"
    if ($LASTEXITCODE -ne 0) { return "tests-failed" }
    Remove-Item "$log-ctest.log"
    return "passed"
}

$cl = Get-Command cl.exe -ErrorAction SilentlyContinue
$results += "Environment: $([System.Environment]::OSVersion.VersionString)"
if ($cl) { $results += "cl.exe $($cl.FileVersionInfo.FileVersion)" } else { $results += "cl.exe not on PATH: run this in a Developer PowerShell" }
foreach ($std in 17, 20) {
    $r = Check "." "build-cpp$std" @("-DCMAKE_CXX_STANDARD=$std") "msvc-cpp$std"
    Row "MSVC C++$std x64 build and CTest (/W4 /WX)" $(if ($r -eq "passed") { "PASS" } else { $r })
}
if (Test-Path "build-cpp17\Release\test_defensive.exe") {
    $results += (& "build-cpp17\Release\test_defensive.exe" data $env:TEMP | Select-Object -Last 1)
}

$mutants = @(
    @("if (end != text.data() + text.size()) {", "if (false) {", "text after a number accepted"),
    @("if (text.size() > max_len) {", "if (text.size() >= max_len) {", "a line of exactly max_len called too long"),
    @("if (b != 0 && a > std::numeric_limits<std::size_t>::max() / b) {", "if (false) {", "no overflow check before multiplying"),
    @(" || in.gcount() > 0) {", ") {", "the last partial block of a copy dropped")
)
if (($results -match "^MSVC C") -notmatch "PASS$") { Row "Mutants" "not run: the unchanged code fails its tests"; $mutants = @() }
$n = 0
foreach ($m in $mutants) {
    $n++
    $copy = Join-Path $env:TEMP "defensive-cpp-mutant-$n"
    if (Test-Path $copy) { Remove-Item -Recurse -Force $copy }
    New-Item -ItemType Directory $copy | Out-Null
    Copy-Item -Recurse CMakeLists.txt, src, tests, data, pitfalls, compile_fail $copy
    $file = Join-Path $copy "src\defensive.cpp"
    $text = [System.IO.File]::ReadAllText($file)
    if (-not $text.Contains($m[0])) { Row "Mutant ${n}: $($m[2])" "NOT APPLIED"; continue }
    [System.IO.File]::WriteAllText($file, $text.Replace($m[0], $m[1]))
    $r = Check $copy (Join-Path $copy "build") @() "msvc-mutant-$n"
    $verdict = switch ($r) { "tests-failed" { "CAUGHT" } "passed" { "MISSED" } default { "BUILD FAILED" } }
    Remove-Item -ErrorAction SilentlyContinue "msvc-mutant-$n-ctest.log"
    Row "Mutant ${n}: $($m[2])" $verdict
    Remove-Item -Recurse -Force $copy
}

$results | Tee-Object -FilePath results-windows.md
