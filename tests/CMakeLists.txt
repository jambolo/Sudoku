cmake_minimum_required (VERSION 3.8)

set(SOURCES
    main.cpp
)

add_executable(tests ${SOURCES})
target_link_libraries(tests Board Generator Solver)
