
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

target_link_libraries(
    parser
    PUBLIC
    pugixml

)


add_executable(
  test.parser.exe
  parser.test.cxx
)

add_test(NAME FINAL_RESULT COMMAND test.parser.exe)
enable_testing()

target_link_libraries(
  test.parser.exe
  GTest::gtest_main
  parser
)

include(GoogleTest)
gtest_discover_tests(test.parser.exe)


