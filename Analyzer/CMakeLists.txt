cmake_minimum_required (VERSION 3.8)

set(SOURCES
    Analyzer.cpp
    Analyzer.h
    Candidates.cpp
    Candidates.h
    Hidden.cpp
    Hidden.h
    Link.cpp
    Link.h
    LockedCandidates.cpp
    LockedCandidates.h
    Naked.cpp
    Naked.h
    XWing.cpp
    XWing.h
    YWing.cpp
    YWing.h
)

set(INTERFACE_INCLUDE_PATHS
    ${PROJECT_SOURCE_DIR}
)

add_library(Analyzer ${SOURCES})
target_include_directories(Analyzer INTERFACE ${INTERFACE_INCLUDE_PATHS})
target_link_libraries(Analyzer PRIVATE Board Solver)