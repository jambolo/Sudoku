#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class YWing
{
public:
    static bool exists(Board const & board,
        Candidates::List const & candidates,
        std::vector<int> & indexes,
        std::vector<int> & values,
        std::string & reason);

private:
    static std::string generateReason(std::vector<int> const & pivots, std::vector<int> const & values);
};


