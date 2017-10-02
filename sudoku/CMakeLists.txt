cmake_minimum_required (VERSION 3.8)

set(SOURCES
    main.cpp
)

add_executable(sudoku ${SOURCES})
target_link_libraries(sudoku Board Generator Solver)
