cmake_minimum_required (VERSION 3.8)

set(SOURCES
    solve.cpp
)

add_executable(solve ${SOURCES})
target_link_libraries(solve Board Solver)
