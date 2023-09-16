set(target_name application)

add_executable(${target_name})

add_compile_options(
    ${target_name}
    -Wall -Wextra -Werror -pedantic
)

target_sources(
    ${target_name}
    PRIVATE
        main.cxx
)

# Link libraries
target_link_libraries(
    ${target_name}
    PRIVATE
        fsp
        cxxopts
)


