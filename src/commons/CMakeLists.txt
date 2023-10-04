
# Make library
add_library(
    parser STATIC
    parser.cxx
    parser.h
)

target_include_directories(
    parser
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

add_executable(
  test-parser
  parser.test.cxx
)

add_test(NAME Fsp_summ COMMAND test-parser)
enable_testing()

target_link_libraries(
  test-parser
  GTest::gtest_main
  parser
)

include(GoogleTest)
gtest_discover_tests(test-parser)


