set(target_name fsp)

# Make library
add_library(
    ${target_name} STATIC
    fsp.cxx
    fsp.h
)

target_include_directories(
    ${target_name}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)



add_executable(
  test-fsp
  fsp.test.cxx
)

add_test(NAME Fsp_summ COMMAND test-fsp)
enable_testing()

target_link_libraries(
  test-fsp
  GTest::gtest_main
  fsp
)

include(GoogleTest)
gtest_discover_tests(test-fsp)


