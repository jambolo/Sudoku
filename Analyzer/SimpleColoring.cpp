#include "SimpleColoring.h"

#include "Candidates.h"
#include "Link.h"

#include "Board/Board.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include <vector>

bool SimpleColoring::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each candidate in each cell:
    //  Form a graph of strong links of the candidate value rooted at the cell.
    //  If two cells in the graph can see each other and the inference of their values are the same, then there is a contradiction.
    // The only possibility is that none of the cells in the graph with the same inference as those two cells can have that
    // candidate.
    //  If any cell with the candidate can see cells in the graph with opposite inferences, then it cannot be a candidate.

    return !Board::ForEach::cell([&] (int i) {
                                     if (Candidates::isSolved(candidates_[i]))
                                         return true;

                                     std::vector<int> cellCandidateValues = Candidates::values(candidates_[i]);
                                     for (auto v : cellCandidateValues)
                                     {
                                         // Separate strong-linked cells into alternating red and green
                                         std::set<int> red;
                                         std::set<int> green;
                                         infer(v, i, red, green);

                                         // If any red cell can see any other red cells, then the value cannot be a candidate in any
                                         // red cells
                                         std::vector<int> collisions;
                                         if (collisionsFound(red, collisions))
                                         {
                                             indexes.insert(indexes.end(), red.begin(), red.end());
                                             values.push_back(v);
                                             reason = generateReason(v, collisions, red);
                                             return false;
                                         }

                                         // If any green cell can see any other green cells, then the value cannot be a candidate in
                                         // any green cells
                                         if (collisionsFound(green, collisions))
                                         {
                                             indexes.insert(indexes.end(), green.begin(), green.end());
                                             values.push_back(v);
                                             reason = generateReason(v, collisions, green);
                                             return false;
                                         }

                                         // If any cell with a candidate v not in the chain can see both a red cell and a green
                                         // cell, then the cell cannot have v as a candidate
                                         int other;
                                         std::vector<int> redDependents;
                                         std::vector<int> greenDependents;
                                         if (canSeeBoth(v, red, green, other, redDependents, greenDependents))
                                         {
                                             indexes.push_back(other);
                                             values.push_back(v);
                                             reason = generateReason(v, other, redDependents, greenDependents);
                                             return false;
                                         }
                                     }
                                     return true;
                                 });
}

std::string SimpleColoring::generateReason(int v, std::vector<int> const & collisions, std::set<int> const & eliminated)
{
    std::string reason = "These squares cannot be ";
    reason += std::to_string(v);
    reason += " because if any of them did, it would lead to a contradiction in which";
    for (auto i : collisions)
    {
        reason += " " + Board::Cell::name(i);
    }
    reason += " would have the same value.";
    return reason;
}

std::string SimpleColoring::generateReason(int v, int i, std::vector<int> const & red, std::vector<int> const & green)
{
    std::vector<int> combined;
    combined.insert(combined.end(), red.begin(), red.end());
    combined.insert(combined.end(), green.begin(), green.end());
    std::sort(combined.begin(), combined.end());

    std::string reason = Board::Cell::name(i) +
                         " cannot be " +
                         std::to_string(v) +
                         " because one of";
    for (auto i1 : combined)
    {
        reason += " " + Board::Cell::name(i1);
    }
    reason += " must be that value.";
    return reason;
}

void SimpleColoring::infer(int v, int i0, std::set<int> & a, std::set<int> & b)
{
    a.insert(i0);
    Link::Strong::List links = Link::Strong::find(candidates_, i0);
    for (auto const & link : links)
    {
        // Only one value at a time for simple coloring
        if (link.v0 != v || link.v1 != v)
            continue;

        // If the other cell hasn't already been included then include it
        if (b.find(link.i1) == b.end())
            infer(v, link.i1, b, a);
    }
}

bool SimpleColoring::collisionsFound(std::set<int> const & indexes, std::vector<int> & collisions)
{
    for (auto i : indexes)
    {
        std::vector<int> const & seen = Board::Cell::dependents(i);
        std::set_intersection(seen.begin(), seen.end(), indexes.begin(), indexes.end(), std::back_inserter(collisions));
        if (!collisions.empty())
            return true;
    }
    return false;
}

bool SimpleColoring::canSeeBoth(int                   v,
                                std::set<int> const & red,
                                std::set<int> const & green,
                                int &                 other,
                                std::vector<int> &    redDependents,
                                std::vector<int> &    greenDependents)
{
    if (!red.empty() && !green.empty())
    {
        // Get all cells with the candidate not in the chain
        std::vector<int> others = Candidates::findAll(candidates_, v);
        {
            std::vector<int> temp;
            std::set_difference(others.begin(), others.end(), red.begin(), red.end(), std::back_inserter(temp));
            others = std::move(temp);
        }
        {
            std::vector<int> temp;
            std::set_difference(others.begin(), others.end(), green.begin(), green.end(), std::back_inserter(temp));
            others = std::move(temp);
        }

        // If any cell not in the chain can see both a red cell and a green cell, then the candidate can be eliminated from that
        // cell
        if (!others.empty())
        {
            for (auto o : others)
            {
                std::vector<int> const & dependents = Board::Cell::dependents(o);
                std::set_intersection(dependents.begin(),
                                      dependents.end(),
                                      red.begin(),
                                      red.end(),
                                      std::back_inserter(redDependents));
                if (!redDependents.empty())
                {
                    std::set_intersection(dependents.begin(),
                                          dependents.end(),
                                          green.begin(),
                                          green.end(),
                                          std::back_inserter(greenDependents));
                    if (!greenDependents.empty())
                    {
                        other = o;
                        return true;
                    }
                    redDependents.clear();
                }
            }
        }
    }
    return false;
}
