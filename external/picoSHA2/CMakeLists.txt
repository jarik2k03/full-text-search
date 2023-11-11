include(FetchContent)

FetchContent_Declare(
  picoSHA2
  GIT_REPOSITORY https://github.com/okdshin/PicoSHA2.git
  GIT_SHALLOW TRUE
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
)

FetchContent_MakeAvailable(picoSHA2)

