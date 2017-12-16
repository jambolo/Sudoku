#include "SimpleColoring.h"

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

bool SimpleColoring::exists(Board const &            board,
                            Candidates::List const & candidates,
                            std::vector<int> &       indexes,
                            std::vector<int> &       values,
                            std::string &            reason)
{
    // Form a graph of strong links of a candidate value rooted at a cell. If two cells in the graph can see each other and the
    // inference of their values are the same, then there is a contradiction. The only possibility is that none of the cells in the
    // graph with the same inference as those two cells can have that candidate.

    return false;
}

std::string SimpleColoring::generateReason()
{
    return "simple coloring";
}
