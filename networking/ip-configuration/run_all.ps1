# run_all.ps1 - the Windows checks behind the article's "What I Verified"
# table: MSVC builds with /W4 /WX /permissive- in C++17 and C++20, every
# CTest test, and deliberate mistakes that a test must catch.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
# Run it as administrator to include change_test, which adds 198.18.77.77
# and fd00:6d79:6370:6c75::77 to the loopback interface and removes them
# again (the addresses do not survive a restart either way).
# Writes the result lines to results-windows.md; if something fails, the
# details are in the msvc-*.log files next to it.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $text) { $script:results += ("{0,-58} {1}" -f $name, $text) }
function Ok($b) { if ($b) { "PASS" } else { "FAIL" } }

$admin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole(
    [Security.Principal.WindowsBuiltInRole]::Administrator)

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
    return "passed"
}

foreach ($std in 17, 20) {
    $r = Check "." "build-msvc-$std" $std "msvc-$std"
    Row "MSVC build C++$std (/W4 /WX /permissive-)" (Ok ($r -ne "build-failed"))
    if ($r -ne "build-failed") {
        Row "MSVC CTest C++$std (7 tests)" (Ok ($r -eq "passed"))
        $change = if (Select-String -Quiet -Path "msvc-$std-ctest.log" -Pattern "change_test .*Skipped") { "skipped (not administrator)" } else { "ran" }
        Row "  change_test C++$std" $change
        if ($r -eq "passed") { Remove-Item "msvc-$std-ctest.log" }
    }
}

# Deliberate mistakes. Each one is applied to a copy of the code; a test must fail.
$mutants = @(
    @("src/ip_text.cpp", "if (j - i > best_length) {", "if (j - i >= best_length) {", "IPv6 compresses the last zero run"),
    @("src/ip_text.cpp", "(s.size() > 1 && s[0] == '0')", "(false)", "leading zeros accepted in IPv4"),
    @("src/ip_text.cpp", "if (bits != expected)", "if (bits == 0 && expected != 0)", "non-contiguous masks accepted"),
    @("src/interfaces_windows.cpp", "in.loopback = adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK;", "in.loopback = false;", "loopback interface not recognized"),
    @("src/interfaces_windows.cpp", "unicast->OnLinkPrefixLength, ip", "32, ip", "every address listed as /32"),
    @("src/interfaces_windows.cpp", "if (lookup != NO_ERROR)", "if (lookup == 12345)", "remove skips the lookup (needs administrator)")
)
# A mutant only counts as caught if the unchanged code passes first.
if ($results -match "CTest .* FAIL") { Row "Mutants" "not run: the unchanged code fails its tests" ; $mutants = @() }
$n = 0
foreach ($m in $mutants) {
    $n++
    if ($m[3] -like "*needs administrator*" -and -not $admin) { Row "Mutant ${n}: $($m[3])" "not run (not administrator)"; continue }
    $copy = Join-Path $env:TEMP "ip-configuration-mutant-$n"
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

$report = @("Environment: " + [System.Environment]::OSVersion.VersionString + $(if ($admin) { " (administrator)" } else { " (not administrator)" }))
$cl = Get-Command cl.exe -ErrorAction SilentlyContinue
if ($cl) { $report += "cl.exe " + $cl.Version }
$report += $results
$report | Set-Content results-windows.md
$report
