cmake_minimum_required (VERSION 3.8)

set(SOURCES
    rate.cpp
)

add_executable(rate ${SOURCES})
target_link_libraries(rate PRIVATE Analyzer Board)
