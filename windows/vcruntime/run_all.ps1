# run_all.ps1 - the Windows checks behind the article's "What I Verified" table.
#   1. Builds the examples for x64, x86 (Win32) and ARM64 with MSVC and runs
#      CTest, which checks with dumpbin which runtime DLLs each program imports.
#   2. Records the imports, the installed Visual C++ runtimes (registry, files,
#      Apps list) and what runtime_check.exe reports.
#   3. Applies deliberate mistakes that a test must catch.
# Run from this directory in a Developer PowerShell (any architecture):  .\run_all.ps1
# Nothing is installed or changed on the machine. The report goes to results-windows.md.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $text) { $script:results += ("{0,-58} {1}" -f $name, $text) }
function Ok($b) { if ($b) { "PASS" } else { "FAIL" } }

# Configures, builds and tests one source tree for one platform.
# Returns "configure-failed", "build-failed", "tests-failed" or "passed".
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

$imports = @()
foreach ($platform in "x64", "Win32", "ARM64") {
    $dir = "build-$platform"
    $r = Check "." $dir $platform "msvc-$platform"
    if ($r -eq "configure-failed" -or $r -eq "build-failed") {
        Row "MSVC $platform build" "$r (are the $platform build tools installed?)"
        continue
    }
    Row "MSVC $platform build (/W4 /WX /permissive-)" "PASS"
    Row "MSVC $platform CTest (imports and output)" (Ok ($r -eq "passed"))
    # Which runtime DLLs each program imports, straight from dumpbin.
    foreach ($exe in "eh_demo", "eh_demo_nofh4", "eh_demo_static", "hello") {
        $path = Join-Path $dir "Release\$exe.exe"
        if (-not (Test-Path $path)) { continue }
        $dlls = (dumpbin /dependents $path | Select-String -Pattern '^\s+(\S+\.dll)$' | ForEach-Object { $_.Matches[0].Groups[1].Value }) -join ", "
        $imports += ("{0,-6} {1,-15} {2}" -f $platform, $exe, $dlls)
    }
}

# Deliberate mistakes (x64 build). A mutant only counts if the unchanged code passed.
$mutants = @(
    @("CMakeLists.txt", "target_compile_options(eh_demo_nofh4 PRIVATE /d2FH4-)", "", "FH4 left on in eh_demo_nofh4"),
    @("CMakeLists.txt", 'set_property(TARGET eh_demo_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")', 'set_property(TARGET eh_demo_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")', "eh_demo_static linked with the DLL runtime"),
    @("CMakeLists.txt", "add_executable(eh_demo src/eh_demo.cpp)", "add_executable(eh_demo src/hello.cpp)", "eh_demo built without exceptions")
)
if ($results -cmatch " FAIL$") { Row "Mutants" "not run: the unchanged code fails its tests"; $mutants = @() }
$n = 0
foreach ($m in $mutants) {
    $n++
    $copy = Join-Path $env:TEMP "vcruntime-mutant-$n"
    if (Test-Path $copy) { Remove-Item -Recurse -Force $copy }
    New-Item -ItemType Directory $copy | Out-Null
    Copy-Item -Recurse src, tests, CMakeLists.txt $copy
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

# What is installed on this machine (read only).
$machine = @("OS architecture: " + $env:PROCESSOR_ARCHITECTURE)
foreach ($arch in "x64", "x86", "arm64") {
    foreach ($root in "HKLM:\SOFTWARE\WOW6432Node", "HKLM:\SOFTWARE") {
        $key = "$root\Microsoft\VisualStudio\14.0\VC\Runtimes\$arch"
        if (Test-Path $key) {
            $p = Get-ItemProperty $key
            $machine += ("registry {0,-6} {1}: Installed={2} Version={3}" -f $arch, $root.Replace("HKLM:\SOFTWARE", "SOFTWARE"), $p.Installed, $p.Version)
        }
    }
}
foreach ($folder in "System32", "SysWOW64") {
    foreach ($dll in "vcruntime140.dll", "vcruntime140_1.dll", "msvcp140.dll") {
        $path = Join-Path $env:windir "$folder\$dll"
        if (Test-Path $path) { $machine += ("{0,-9} {1,-20} {2}" -f $folder, $dll, (Get-Item $path).VersionInfo.FileVersion) }
        else { $machine += ("{0,-9} {1,-20} not present" -f $folder, $dll) }
    }
}
$uninstall = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*", "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*"
Get-ItemProperty $uninstall -ErrorAction SilentlyContinue |
    Where-Object { $_.DisplayName -like "Microsoft Visual C++*Redistributable*" } |
    Sort-Object DisplayName | ForEach-Object { $machine += ("Apps list: {0} {1}" -f $_.DisplayName, $_.DisplayVersion) }
foreach ($platform in "x64", "Win32") {
    $checker = "build-$platform\Release\runtime_check.exe"
    if (Test-Path $checker) { $machine += "runtime_check ($platform build):"; $machine += (& $checker) }
}

$report = @("Environment: " + [System.Environment]::OSVersion.VersionString)
$cl = Get-Command cl.exe -ErrorAction SilentlyContinue
if ($cl) { $report += "cl.exe " + $cl.Version }
$report += $results
$report += "Imports (dumpbin /dependents):"
$report += $imports
$report += "This machine:"
$report += $machine
$report | Set-Content results-windows.md
$report
