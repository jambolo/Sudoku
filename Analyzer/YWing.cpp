#include "YWing.h"

#include "Candidates.h"
#include "Link.h"

#include "Board/Board.h"

#include <cassert>
#include <string>
#include <vector>

bool YWing::exists(Board const & board,
    Candidates::List const & candidates,
    std::vector<int> & indexes,
    std::vector<int> & values,
    std::string & reason)
{
    // If a cell has a strong link of value A in a cell with only two candidates (A,C), and a strong link
    // of a value B in another cell with two candidates (B,C), and the other candidate in those two
    // cells are the value C, then any cells that can see both of those cells cannot have the value C
    // because one of those two cells, (A,C) or (B,C), must have the value C.

    std::vector<int> pivots;
    std::vector<int> pivotValues;

    bool found = !Board::ForEach::cell([&](int i) {
        // Get all the strong links for this cell
        Link::Strong::List links = Link::Strong::find(candidates, i);

        if (!links.empty())
        {
            int i0 = i;
            for (Link::Strong::List::const_iterator link1 = links.begin(); link1 != std::prev(links.end()); ++link1)
            {
                int i1 = link1->i1;
                int v1 = link1->value;
                if (Candidates::count(candidates[i1]) == 2)
                {
                    for (Link::Strong::List::const_iterator link2 = std::next(link1); link2 != links.end(); ++link2)
                    {
                        int i2 = link2->i1;
                        int v2 = link2->value;
                        if (i1 != i2 && v1 != v2 && Candidates::count(candidates[i2]) == 2)
                        {
                            Candidates::Type mask3 = (candidates[i1] & ~Candidates::fromValue(v1)) &
                                                     (candidates[i2] & ~Candidates::fromValue(v2));
                            if (mask3)
                            {
                                assert(Candidates::count(mask3) == 1);
                                // A Y-wing has been found. The candidate v3 can be removed from all cells that can
                                // see both i1 and i2.
                                int v3 = Candidates::value(mask3);
                                std::vector<int> seen = Board::Cell::dependents(i1, i2);
                                for (int s : seen)
                                {
                                    if (candidates[s] & mask3)
                                        indexes.push_back(s);
                                }
                                if (!indexes.empty())
                                {
                                    values.push_back(v3);
                                    pivots = { i0, i1, i2 };
                                    pivotValues = { v1, v2, v3 };
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }); 

    if (found)
        reason = generateReason(pivots, pivotValues);
    return found;
}

std::string YWing::generateReason(std::vector<int> const & pivots, std::vector<int> const & values)
{
    std::string reason = "If square " +
                        Board::Cell::name(pivots[0]) +
                        " is " +
                        std::to_string(values[0]) +
                        ", then square " +
                        Board::Cell::name(pivots[1]) +
                        " must be " +
                        std::to_string(values[2]) +
                        ", or if square " +
                        Board::Cell::name(pivots[0]) +
                        " is " +
                        std::to_string(values[1]) +
                        " then square " +
                        Board::Cell::name(pivots[2]) +
                        " must be " +
                        std::to_string(values[2]) +
                        ". Either way, none of these squares can be " + 
                        std::to_string(values[2]) +
                        ".";
        return reason;
}
