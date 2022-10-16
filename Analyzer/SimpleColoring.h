#if !defined(ANALYZER_SIMPLECOLORING_H_INCLUDED)
#define ANALYZER_SIMPLECOLORING_H_INCLUDED 1
#pragma once

#include "Candidates.h"
#include <set>
#include <string>
#include <vector>

class SimpleColoring
{
public:
    SimpleColoring(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if a simple coloring elimination exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    static std::string generateReason(int v, std::vector<int> const & collisions, std::set<int> const & eliminated);
    static std::string generateReason(int v, int i, std::vector<int> const & red, std::vector<int> const & green);
    void               infer(int v, int i0, std::set<int> & a, std::set<int> & b);
    bool               collisionsFound(std::set<int> const & indexes, std::vector<int> & collisions);
    bool               canSeeBoth(int                   v,
                                  std::set<int> const & red,
                                  std::set<int> const & green,
                                  int &                 other,
                                  std::vector<int> &    redDependents,
                                  std::vector<int> &    greenDependents);

    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_SIMPLECOLORING_H_INCLUDED)
