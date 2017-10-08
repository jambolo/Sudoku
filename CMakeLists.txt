cmake_minimum_required (VERSION 3.8)
project(Sudoku CXX)


add_definitions(
    -DNOMINMAX
    -DWIN32_LEAN_AND_MEAN
    -DVC_EXTRALEAN
    -D_CRT_SECURE_NO_WARNINGS
    -D_SECURE_SCL=0
    -D_SCL_SECURE_NO_WARNINGS
)

option(BUILD_SHARED_LIBS "Build libraries as DLLs" FALSE)

# Libraries

add_subdirectory(Analyzer)
add_subdirectory(Board)
add_subdirectory(Generator)
add_subdirectory(Solver)

# Applications and tests

add_subdirectory(generate)
add_subdirectory(solve)
add_subdirectory(tests)
add_subdirectory(tutor)
