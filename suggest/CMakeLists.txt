cmake_minimum_required (VERSION 3.8)

set(SOURCES
    suggest.cpp
)

add_executable(suggest ${SOURCES})
target_link_libraries(suggest PRIVATE Analyzer Board Solver nlohmann_json::nlohmann_json)
