# run_all.ps1 - the Windows checks behind the article's "What I Verified"
# table: an MSVC build with /W4 /WX, every CTest test, and the benchmark.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
# Writes the result lines to results-windows.md; if the build fails, the
# compiler output is in msvc-build.log.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $ok) { $script:results += ("{0,-44} {1}" -f $name, $(if ($ok) { "PASS" } else { "FAIL" })) }

if (Test-Path build-msvc) { Remove-Item -Recurse -Force build-msvc }
cmake -S . -B build-msvc *> msvc-build.log
$built = $LASTEXITCODE -eq 0
if ($built) { cmake --build build-msvc --config Release *>> msvc-build.log; $built = $LASTEXITCODE -eq 0 }
Row "MSVC build (/W4 /WX /permissive-)" $built
$bench = @()
if ($built) {
    Remove-Item msvc-build.log
    ctest --test-dir build-msvc -C Release --output-on-failure
    Row "MSVC CTest (tests, predictions, checksum)" ($LASTEXITCODE -eq 0)
    $bench = & .\build-msvc\Release\tree_bench.exe 7
}

$report = @("Environment: " + [System.Environment]::OSVersion.VersionString)
$report += (Get-Command cl.exe -ErrorAction SilentlyContinue | ForEach-Object { "cl.exe " + $_.Version })
$report += (Get-CimInstance Win32_Processor -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { "CPU: " + $_.Name })
$report += $results
$report += ""
$report += "tree_bench (MSVC, Release):"
$report += $bench
$report | Set-Content results-windows.md
$report
