cmake_minimum_required (VERSION 3.10)
project(Sudoku CXX)

option(BUILD_SHARED_LIBS "Build libraries as DLLs" FALSE)

# Common compiler definitions

add_definitions(
    -DNOMINMAX
    -DWIN32_LEAN_AND_MEAN
    -DVC_EXTRALEAN
    -D_CRT_SECURE_NO_WARNINGS
    -D_SECURE_SCL=0
    -D_SCL_SECURE_NO_WARNINGS
)

# Common external libraries

find_package(nlohmann_json REQUIRED)

# Libraries

add_subdirectory(Analyzer)
add_subdirectory(Board)
add_subdirectory(Generator)
add_subdirectory(Solver)

# Applications

add_subdirectory(generate)
add_subdirectory(rate)
add_subdirectory(solve)
add_subdirectory(suggest)
add_subdirectory(profile)

#########################################################################
# Testing                                                               #
#########################################################################

include(CTest)
message(STATUS "Testing is enabled. Turn on BUILD_TESTING to build tests.")
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
