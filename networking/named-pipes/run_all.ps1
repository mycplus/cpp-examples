# run_all.ps1 - the Windows checks behind the article's "What I Verified"
# table: MSVC builds with /W4 /WX /permissive- in C++17 and C++20, every CTest
# scenario, and the deliberate-mistake checks (each mutant must fail a test).
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
    if ($r -ne "build-failed") { Row "MSVC CTest C++$std (7 client/server scenarios)" (Ok ($r -eq "passed")) }
}

# Deliberate mistakes. Each one is applied to a copy of the code; a test must fail.
$mutants = @(
    @("src/pipe_server.cpp", "DisconnectNamedPipe(pipe.get());           // ready for the next client", "// (DisconnectNamedPipe removed)", "simple server never disconnects"),
    @("src/pipe_client.cpp", "if (!SetNamedPipeHandleState(pipe.get(), &mode, nullptr, nullptr)) {", "if (mode == 0) {", "client left in byte mode"),
    @("src/pipe_common.h", "if (e == ERROR_MORE_DATA) {            // the message is longer than the buffer", "if (e == ERROR_MORE_DATA) { got = 0;", "read_message drops a partial chunk"),
    @("src/overlapped_server.cpp", "if (error == ERROR_MORE_DATA) {", "if (bytes == 0) {", "overlapped server ignores ERROR_MORE_DATA"),
    @("src/thread_server.cpp", "worker.join();", "worker.detach();", "thread server exits without joining"),
    @("src/thread_server.cpp", "(id == 1 ? FILE_FLAG_FIRST_PIPE_INSTANCE : 0)", "0", "no FILE_FLAG_FIRST_PIPE_INSTANCE")
)
$n = 0
foreach ($m in $mutants) {
    $n++
    $copy = Join-Path $env:TEMP "named-pipes-mutant-$n"
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
$report | Set-Content results-windows.md
$report
