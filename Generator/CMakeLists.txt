cmake_minimum_required (VERSION 3.8)

set(SOURCES
    Generator.cpp
    Generator.h
)

set(INTERFACE_INCLUDE_PATHS
    ${PROJECT_SOURCE_DIR}
)

add_library(Generator ${SOURCES})
target_include_directories(Generator INTERFACE ${INTERFACE_INCLUDE_PATHS})
target_link_libraries(Generator Board Solver)