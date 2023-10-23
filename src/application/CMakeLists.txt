add_executable(ft-searcher)

add_compile_options(
    ft-searcher
    -Wall -Wextra -Werror -pedantic
)

target_sources(
    ft-searcher
    PRIVATE
        main.cxx
)

# Link libraries
target_link_libraries(
    ft-searcher
    PRIVATE
        indexer
        parser
        cxxopts
        pugixml
)


