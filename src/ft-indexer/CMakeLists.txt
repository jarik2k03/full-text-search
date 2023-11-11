add_library(
    indexer STATIC
    indexer.cxx
    indexer.h
)

target_include_directories(
    indexer
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    indexer
    PUBLIC
    picosha2
    parser
)
 
# TEEEEST
add_executable(
  test.indexer.exe
  indexer.test.cxx
)

add_test(NAME Indexer_FINAL_RESULT COMMAND test.indexer.exe)
enable_testing()

target_link_libraries(
  test.indexer.exe
  GTest::gtest_main
  indexer
)

include(GoogleTest)
gtest_discover_tests(test.indexer.exe)