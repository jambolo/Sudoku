cmake_minimum_required (VERSION 3.8)

set(SOURCES
    main.cpp
)

add_executable(profile ${SOURCES})
target_link_libraries(profile PRIVATE Analyzer Board Generator Solver)
