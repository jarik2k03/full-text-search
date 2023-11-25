add_library(
    searcher STATIC
    searcher.cxx
    searcher.h
)

target_include_directories(
    searcher
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    searcher
    PUBLIC
    picosha2
    parser
)
 
# TEEEEST
add_executable(
  test.searcher.exe
  searcher.test.cxx
)

add_test(NAME Searcher_FINAL_RESULT COMMAND test.searcher.exe)
enable_testing()

target_link_libraries(
  test.searcher.exe
  GTest::gtest_main
  searcher
)

include(GoogleTest)
gtest_discover_tests(test.searcher.exe)