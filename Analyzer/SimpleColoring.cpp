#include "SimpleColoring.h"

#include "Candidates.h"
#include "Link.h"

#include "Board/Board.h"

#include <set>
#include <string>
#include <vector>

bool SimpleColoring::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // Form a graph of strong links of a candidate value rooted at a cell. If two cells in the graph can see each other and the
    // inference of their values are the same, then there is a contradiction. The only possibility is that none of the cells in the
    // graph with the same inference as those two cells can have that candidate.

    return !Board::ForEach::cell([&] (int i) {
        std::vector<int> values = Candidates::values(candidates_[i]);
        for (int v : values)
        {
            // Separate linked cells into red and green
            std::set<int> red;
            std::set<int> green;
            infer(v, i, red, green);
            
            // If any red cell can see any other red cells, then the value cannot be a candidate in any red cells
            std::vector<int> collisions;
            if (collisionsFound(red, collisions))
            {
                indexes.insert(indexes.end(), red.begin(), red.end());
                values.push_back(v);
                reason = generateReason(v, collisions, red);
                return false;
            }

            // If any green cell can see any other green cells, then the value cannot be a candidate in any green cells
            if (collisionsFound(green, collisions))
            {
                indexes.insert(indexes.end(), green.begin(), green.end());
                values.push_back(v);
                reason = generateReason(v, collisions, green);
                return false;
            }
            
            // If any cell with a candidate v can see both a red cell and a green cell, then the cell cannot have v as a candidate
        }
        return true;
    });
}

std::string SimpleColoring::generateReason(int v, std::vector<int> const & collisions, std::set<int> const & eliminated)
{
    return "simple coloring";
}

void SimpleColoring::infer(int v, int i0, std::set<int> & a, std::set<int> & b)
{
    a.insert(i0);
    Link::Strong::List links = Link::Strong::find(candidates_, i0);
    for (auto const & link : links)
    {
        if (link.value == v && b.find(link.i1) == b.end())
            infer(v, link.i1, b, a);
    }
}

bool SimpleColoring::collisionsFound(std::set<int> const & indexes, std::vector<int> & collisions)
{
    for (int i : indexes)
    {
        std::vector<int> const & seen = Board::Cell::dependents(i);
        std::set_intersection(seen.begin(), seen.end(), indexes.begin(), indexes.end(), std::back_inserter(collisions));
        if (!collisions.empty())
            return true;
    }
    return false;
}
