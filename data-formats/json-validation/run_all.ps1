# run_all.ps1 - the Windows checks behind the article's "What I Verified" table.
#   1. Builds the examples with MSVC (x64 and Win32, /W4 /WX) and runs CTest,
#      which compares each program's output with tests/expected.
#   2. Applies deliberate mistakes that a test must catch.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
# CMake downloads nlohmann/json 3.12.0 from GitHub on the first build.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $text) { $script:results += ("{0,-58} {1}" -f $name, $text) }

# Configures, builds and tests one source tree. Returns "configure-failed",
# "build-failed", "tests-failed" or "passed".
function Check($src, $dir, $platform, $log) {
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    cmake -S $src -B $dir -A $platform *> "$log-build.log"
    if ($LASTEXITCODE -ne 0) { return "configure-failed" }
    cmake --build $dir --config Release *>> "$log-build.log"
    if ($LASTEXITCODE -ne 0) { return "build-failed" }
    Remove-Item "$log-build.log"
    ctest --test-dir $dir -C Release --output-on-failure *> "$log-ctest.log"
    if ($LASTEXITCODE -ne 0) { return "tests-failed" }
    Remove-Item "$log-ctest.log"
    return "passed"
}

$cl = (Get-Command cl.exe -ErrorAction SilentlyContinue)
$results += "Environment: $([System.Environment]::OSVersion.VersionString)"
if ($cl) { $results += "cl.exe $($cl.FileVersionInfo.FileVersion)" }
foreach ($platform in "x64", "Win32") {
    $r = Check "." "build-$platform" $platform "msvc-$platform"
    Row "MSVC $platform build and CTest (/W4 /WX /permissive-)" $(if ($r -eq "passed") { "PASS" } else { $r })
}

# Deliberate mistakes (x64). A mutant only counts if the unchanged code passed.
$mutants = @(
    @("src\validate_json.cpp", "json::parse(text);", "json::parse(text, nullptr, true, true);", "validator ignores comments"),
    @("src\validate_user.cpp", "if (known.count(item.key()) == 0)", "if (false)", "unknown fields allowed"),
    @("src\validate_user.cpp", " || score.get<double>() < 0 || score.get<double>() > 100", "", "score range not checked")
)
if (($results -cmatch "Win32|x64") -notmatch "PASS$") { Row "Mutants" "not run: the unchanged code fails its tests"; $mutants = @() }
$n = 0
foreach ($m in $mutants) {
    $n++
    $copy = Join-Path $env:TEMP "json-validation-mutant-$n"
    if (Test-Path $copy) { Remove-Item -Recurse -Force $copy }
    New-Item -ItemType Directory $copy | Out-Null
    Copy-Item -Recurse src, tests, cases, CMakeLists.txt $copy
    $file = Join-Path $copy $m[0]
    $text = [System.IO.File]::ReadAllText($file)
    if (-not $text.Contains($m[1])) { Row "Mutant ${n}: $($m[3])" "NOT APPLIED"; continue }
    [System.IO.File]::WriteAllText($file, $text.Replace($m[1], $m[2]))
    $r = Check $copy (Join-Path $copy "build") "x64" "msvc-mutant-$n"
    $verdict = switch ($r) { "tests-failed" { "CAUGHT" } "passed" { "MISSED" } default { "BUILD FAILED" } }
    Remove-Item -ErrorAction SilentlyContinue "msvc-mutant-$n-ctest.log"
    Row "Mutant ${n}: $($m[3])" $verdict
    Remove-Item -Recurse -Force $copy
}

# Two lines of real output, for the article.
Push-Location cases
$results += "validate_json (x64):"
$results += (& ..\build-x64\Release\validate_json.exe valid\user.json invalid\trailing-comma.json)
Pop-Location

$results | Tee-Object -FilePath results-windows.md
