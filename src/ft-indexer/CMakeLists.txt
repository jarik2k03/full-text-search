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
    pugixml

)

# TEEEEST
add_executable(
  test-indexer
  indexer.test.cxx
)

add_test(NAME FINAL_RESULT COMMAND test-indexer)
enable_testing()

target_link_libraries(
  test-indexer
  GTest::gtest_main
  indexer
)

include(GoogleTest)
gtest_discover_tests(test-indexer)