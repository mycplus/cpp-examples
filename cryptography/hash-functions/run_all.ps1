# Build and test with MSVC. Run:  .\run_all.ps1
# Results go to the screen; redirect them to results-windows.md if you want a file to send back.
$ErrorActionPreference = 'Continue'
$failed = 0
foreach ($std in 17,20) {
    $dir = "build-cxx$std"
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    cmake -S . -B $dir "-DCMAKE_CXX_STANDARD=$std" *> "msvc-configure-cxx$std.log"
    if ($LASTEXITCODE -ne 0) { Write-Output "cxx$std : configure FAILED (see msvc-configure-cxx$std.log)"; $failed++; continue }
    cmake --build $dir --config Release *> "msvc-build-cxx$std.log"
    if ($LASTEXITCODE -ne 0) { Write-Output "cxx$std : build FAILED (see msvc-build-cxx$std.log)"; $failed++; continue }
    ctest --test-dir $dir -C Release --output-on-failure *> "msvc-test-cxx$std.log"
    if ($LASTEXITCODE -ne 0) { Write-Output "cxx$std : tests FAILED (see msvc-test-cxx$std.log)"; $failed++ }
    else { Write-Output "cxx$std : MSVC build and tests passed" }
}
exit $failed
