# Make library
add_library(
    configurator STATIC
    configurator.cxx
    configurator.h
)
add_library(
    parser STATIC
    parser.cxx
    parser.h
)

target_include_directories(
    configurator
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)
target_include_directories(
    parser
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    configurator
    PUBLIC
    pugixml
)
target_link_libraries(
    parser
    PUBLIC
    configurator
)

add_executable(
  test.parser.exe
  parser.test.cxx
)
add_executable(
  test.configurator.exe
  configurator.test.cxx
)

add_test(NAME Parser_FINAL_RESULT COMMAND test.parser.exe)
add_test(NAME Configurator_FINAL_RESULT COMMAND test.configurator.exe)
enable_testing()

target_link_libraries(
  test.parser.exe
  GTest::gtest_main
  parser
)
target_link_libraries(
  test.configurator.exe
  GTest::gtest_main
  configurator
)

include(GoogleTest)
gtest_discover_tests(test.parser.exe)
gtest_discover_tests(test.configurator.exe)

