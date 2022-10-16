#include "XYWing.h"

#include "Candidates.h"
#include "Link.h"

#include "Board/Board.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

bool XYWing::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // If a cell has exactly two candidates (v1,v2), and can see one cell with exactly two candidates (v1,v3) and another cell with
    // exactly two candidates (v2,v3), then all cells that can see both of those two cells cannot have the candidate v3.

    std::vector<int> pivots;
    std::vector<int> pivotValues;

    bool found = !Board::ForEach::cell([&] (int i) {
                                           return existsAt(i, pivots, pivotValues, indexes, values, reason);
                                       });

    if (found)
        reason = generateReason(pivots, pivotValues, indexes);
    return found;
}

bool XYWing::existsAt(int                i0,
                      std::vector<int> & pivots,
                      std::vector<int> & pivotValues,
                      std::vector<int> & indexes,
                      std::vector<int> & values,
                      std::string &      reason)
{
    // Candidates::Type candidates0 = candidates_[i0];

    if (!Candidates::isBivalue(candidates_[i0]))
        return true;

    Link::Weak::List links = Link::Weak::find(candidates_, i0);

    // Remove all links to cells that do not have exactly two candidates
    links.erase(std::remove_if(links.begin(), links.end(), [&](Link::Weak const & link) {
                                   return !Candidates::isBivalue(candidates_[link.i1]);
                               }), links.end());

    if (links.empty())
        return true;

    for (Link::Weak::List::const_iterator link1 = links.begin();
         link1 != std::prev(links.end()); ++link1)
    {
        int i1 = link1->i1;
        int v1 = link1->v0;
        Candidates::Type candidates1 = candidates_[i1];

        for (Link::Weak::List::const_iterator link2 = std::next(link1); link2 != links.end();
             ++link2)
        {
            int i2 = link2->i1;
            int v2 = link2->v0;
            if (i1 == i2 || v1 == v2)
                continue;

            Candidates::Type candidates2 = candidates_[i2];
            Candidates::Type candidates3 = (candidates1 & ~Candidates::fromValue(v1)) &
                                           (candidates2 & ~Candidates::fromValue(v2));
            if (candidates3)
            {
                assert(Candidates::count(candidates3) == 1);
                // A Y-wing has been found. The candidate v3 can be removed from all cells
                // that can
                // see both i1 and i2.
                int v3 = Candidates::value(candidates3);
                std::vector<int> seen = Board::Cell::dependents(i1, i2);
                for (auto s : seen)
                {
                    if (candidates_[s] & candidates3)
                        indexes.push_back(s);
                }
                if (!indexes.empty())
                {
                    values.push_back(v3);
                    pivots      = { i0, i1, i2 };
                    pivotValues = { v1, v2, v3 };
                    return false;
                }
            }
        }
    }
    return true;
}

std::string XYWing::generateReason(std::vector<int> const & pivots,
                                   std::vector<int> const & values,
                                   std::vector<int> const & eliminated)
{
    std::string reason;
    reason = "If square " +
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
             ". Either way, ";
    for (auto e : eliminated)
    {
        reason += Board::Cell::name(e);
        reason += ' ';
    }
    reason += "cannot be " +
              std::to_string(values[2]) +
              ".";
    return reason;
}
