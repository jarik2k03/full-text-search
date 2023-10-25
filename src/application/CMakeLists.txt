add_executable(ft-searcher.exe)

add_compile_options(
    ft-searcher.exe
    -Wall -Wextra -Werror -pedantic
)

target_sources(
    ft-searcher.exe
    PRIVATE
        main.cxx
)

# Link libraries
target_link_libraries(
    ft-searcher.exe
    PRIVATE
        indexer
        parser
        cxxopts
)


