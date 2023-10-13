add_executable(application)

add_compile_options(
    application
    -Wall -Wextra -Werror -pedantic
)

target_sources(
    application
    PRIVATE
        main.cxx
)

# Link libraries
target_link_libraries(
    application
    PRIVATE
        parser
        cxxopts
        pugixml
)


