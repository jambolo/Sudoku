#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class XYWing
{
public:
    XYWing(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if a xy-wing exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    static std::string generateReason(std::vector<int> const & pivots, std::vector<int> const & values);

    Candidates::List const & candidates_;
};
