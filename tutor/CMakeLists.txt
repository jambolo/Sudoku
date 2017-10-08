cmake_minimum_required (VERSION 3.8)

set(SOURCES
    tutor.cpp
)

add_executable(tutor ${SOURCES})
target_link_libraries(tutor PRIVATE Analyzer Board Solver)
