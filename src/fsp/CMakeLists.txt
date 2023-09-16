set(target_name fsp)

# Make library
add_library(
    ${target_name} STATIC
    fsp.cxx
    fsp.h
)

add_compile_options(
    ${target_name}
    -Wextra
    -Werror
    -Wall
    -pedantic
)

target_include_directories(
    ${target_name}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
