#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class YWing
{
public:
    YWing(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if a y-wing exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    static std::string generateReason(std::vector<int> const & pivots, std::vector<int> const & values);

    Candidates::List const & candidates_;
};
