cmake_minimum_required (VERSION 3.8)

set(SOURCES
    Board.cpp
    Board.h
)

set(INTERFACE_INCLUDE_PATHS
    ${PROJECT_SOURCE_DIR}
)

add_library(Board ${SOURCES})
target_include_directories(Board INTERFACE ${INTERFACE_INCLUDE_PATHS})
