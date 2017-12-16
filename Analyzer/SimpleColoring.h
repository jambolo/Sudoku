#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class SimpleColoring
{
public:
    static bool exists(Board const &            board,
                       Candidates::List const & candidates,
                       std::vector<int> &       indexes,
                       std::vector<int> &       values,
                       std::string &            reason);

private:
    static std::string generateReason();
};
