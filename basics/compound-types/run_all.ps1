# run_all.ps1 - the Windows checks behind the article's "What I Verified"
# table: an MSVC build with /W4 /WX /permissive- and every CTest test,
# including the two programs that must not compile.
# Run from this directory in a Developer PowerShell:  .\run_all.ps1
# Writes the result lines to results-windows.md; if the build fails, the
# compiler output is in msvc-build.log, and failing tests are in msvc-ctest.log.
Set-Location $PSScriptRoot
$results = @()
function Row($name, $ok) { $script:results += ("{0,-48} {1}" -f $name, $(if ($ok) { "PASS" } else { "FAIL" })) }

foreach ($std in 17, 20) {
    $dir = "build-msvc-$std"
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    cmake -S . -B $dir "-DCMAKE_CXX_STANDARD=$std" *> "msvc-build-$std.log"
    $built = $LASTEXITCODE -eq 0
    if ($built) { cmake --build $dir --config Release *>> "msvc-build-$std.log"; $built = $LASTEXITCODE -eq 0 }
    Row "MSVC build C++$std (/W4 /WX /permissive-)" $built
    if ($built) {
        Remove-Item "msvc-build-$std.log"
        ctest --test-dir $dir -C Release --output-on-failure *> "msvc-ctest-$std.log"
        $passed = $LASTEXITCODE -eq 0
        Row "MSVC CTest C++$std (outputs, compile failures)" $passed
        if ($passed) { Remove-Item "msvc-ctest-$std.log" }
    }
}

$report = @("Environment: " + [System.Environment]::OSVersion.VersionString)
$report += (Get-Command cl.exe -ErrorAction SilentlyContinue | ForEach-Object { "cl.exe " + $_.Version })
$report += $results
$report | Set-Content results-windows.md
$report
