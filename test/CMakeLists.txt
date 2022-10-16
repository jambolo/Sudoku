cmake_minimum_required (VERSION 3.8)

set(GTEST_ROOT "" CACHE PATH "Location of GTest")
find_package(GTest REQUIRED)
include(GoogleTest)
find_package(nlohmann_json REQUIRED)

add_definitions(
    -DNOMINMAX
    -DWIN32_LEAN_AND_MEAN
    -DVC_EXTRALEAN
    -D_CRT_SECURE_NO_WARNINGS
    -D_SECURE_SCL=0
    -D_SCL_SECURE_NO_WARNINGS
)

set(LIBRARIES Analyzer Board Generator Solver)

set(SOURCES
    test-Analyzer_Analyzer.cpp
    test-Analyzer_Candidates.cpp
    test-Analyzer_Hidden.cpp
    test-Analyzer_Link.cpp
    test-Analyzer_LockedCandidates.cpp
    test-Analyzer_Naked.cpp
    test-Analyzer_SimpleColoring.cpp
    test-Analyzer_XWing.cpp
    test-Analyzer_XYWing.cpp
    test-Analyzer_UniqueRectangle.cpp

    test-Board_Board.cpp

    test-Generator_Generator.cpp

    test-Solver_Solver.cpp
)

foreach(FILE ${SOURCES})
    get_filename_component(TEST ${FILE} NAME_WE)
    set(TEST_EXE "${PROJECT_NAME}_${TEST}")
    add_executable(${TEST_EXE} ${FILE})
    target_link_libraries(${TEST_EXE} PRIVATE ${LIBRARIES} GTest::GTest GTest::Main nlohmann_json::nlohmann_json)
    gtest_discover_tests(${TEST_EXE})
    target_compile_features(${TEST_EXE} PRIVATE cxx_std_17)
    set_target_properties(${TEST_EXE} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()
