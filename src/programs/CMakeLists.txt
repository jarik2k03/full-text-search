


add_executable(ft-indexer.exe)
add_executable(ft-searcher.exe)

target_sources(
    ft-indexer.exe
    PRIVATE
        main.indexer.cxx
)

# Link libraries
target_link_libraries(
    ft-indexer.exe
    PRIVATE
        configurator
        indexer
        parser
        cxxopts
)

add_compile_options(
    ft-indexer.exe
    -Wall -Wextra -Werror -pedantic
)
add_compile_options(
    ft-searcher.exe
    -Wall -Wextra -Werror -pedantic
)

target_sources(
    ft-searcher.exe
    PRIVATE
        main.searcher.cxx
)

# Link libraries
target_link_libraries(
    ft-searcher.exe
    PRIVATE
        configurator
        searcher
        parser
        cxxopts
        replxx
)