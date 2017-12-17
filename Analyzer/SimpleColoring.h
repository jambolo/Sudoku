#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <set>
#include <string>
#include <vector>

class SimpleColoring
{
public:
    SimpleColoring(Candidates::List const & candidates) : candidates_(candidates) {}
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    std::string generateReason(int v, std::vector<int> const & collisions, std::set<int> const & eliminated);
    void infer(int v, int i0, std::set<int> & a, std::set<int> & b);
    bool collisionsFound(std::set<int> const & indexes, std::vector<int> & collisions);
    
    Candidates::List const & candidates_;
};
