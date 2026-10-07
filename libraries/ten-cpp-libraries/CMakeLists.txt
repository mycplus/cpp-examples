cmake_minimum_required(VERSION 3.22)
project(ten_cpp_libraries CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)        # Qt 6 requires it

# Every library comes from the system's packages (Ubuntu 24.04: see README).
find_package(Boost 1.74 REQUIRED)               # header-only parts only
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(CURL REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)
find_package(Catch2 3 REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation JSON)
find_package(fmt REQUIRED)

set(STRICT -Wall -Wextra -pedantic -Werror)

function(example name)
  add_executable(${name} src/${name}.cpp)
  target_link_libraries(${name} PRIVATE ${ARGN})
  target_compile_options(${name} PRIVATE ${STRICT})
endfunction()

example(stl_ranges)
example(boost_multiprecision Boost::headers)
example(qt_widgets Qt6::Widgets)
example(eigen_solve Eigen3::Eigen)
example(curl_fetch CURL::libcurl)
example(tbb_parallel TBB::tbb)
example(opencv_edges opencv_core opencv_imgproc opencv_imgcodecs)
example(catch2_tests Catch2::Catch2WithMain)
example(poco_json Poco::Foundation Poco::JSON)
example(fmt_format fmt::fmt)

enable_testing()
function(expect_output name)
  add_test(NAME ${name}_output COMMAND ${CMAKE_COMMAND}
    -DPROG=$<TARGET_FILE:${name}> "-DARGS=${ARGN}"
    -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected/${name}.txt
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/compare_output.cmake)
endfunction()
foreach(prog stl_ranges boost_multiprecision eigen_solve curl_fetch tbb_parallel
             opencv_edges poco_json fmt_format qt_widgets)
  expect_output(${prog})
endforeach()
set_tests_properties(qt_widgets_output PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
# Catch2 reports its own pass/fail through the exit status.
add_test(NAME catch2_tests COMMAND catch2_tests)
