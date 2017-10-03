cmake_minimum_required (VERSION 3.8)

set(SOURCES
    generate.cpp
)

add_executable(generate ${SOURCES})
target_link_libraries(generate Board Generator Solver)
