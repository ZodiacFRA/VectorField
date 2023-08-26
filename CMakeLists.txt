cmake_minimum_required(VERSION 3.18)
project(Windy)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -ggdb -Wunreachable-code)

add_executable(Windy
        main.cpp
        App.cpp
        noiseGenerator.cpp
        Boat.cpp
        # deps/SimplexNoise.cpp
)
include_directories(./deps)

find_package(SFML 2.5 REQUIRED audio graphics window system)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(Windy sfml-graphics sfml-audio sfml-system sfml-window)
