cmake_minimum_required (VERSION 3.8)

set(SOURCES
    Solver.cpp
    Solver.h
)

set(INTERFACE_INCLUDE_PATHS
    ${PROJECT_SOURCE_DIR}
)

add_library(Solver ${SOURCES})
target_include_directories(Solver INTERFACE ${INTERFACE_INCLUDE_PATHS})
target_link_libraries(Solver Board)