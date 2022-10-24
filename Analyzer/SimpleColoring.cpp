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
    // For each candidate in each cell that has not already been checked:
    //  Form a graph of strong links of the candidate value rooted at the cell.
    //  1. If two cells in the graph can see each other and the inference of their values are the same, then there is a
    // contradiction.
    // The only possibility is that none of the cells in the graph with the same inference as those two cells can have that
    // candidate.
    //  2. If any cell with the candidate can see cells in the graph with opposite inferences, then it cannot be a candidate.

    for (int v = 1; v <= 9; ++v)
    {
        std::vector<bool> alreadyChecked(Board::NUM_CELLS, false);
        for (int i = 0; i < Board::NUM_CELLS; ++i)
        {
            Candidates::Type c = candidates_[i];
            if (!Candidates::includes(c, v) || Candidates::isSolved(c) || alreadyChecked[i])
                continue;

            // Separate strong-linked cells into alternating red and green
            std::set<int> red;
            std::set<int> green;
            createGraph(v, i, red, green);

            // Optimization: Ensure that the linked cells are not checked for this value again.
            for (auto i : red)
            {
                alreadyChecked[i] = true;
            }
            for (auto i : green)
            {
                alreadyChecked[i] = true;
            }

            // If any red cell can see any other red cell, then the value cannot be a candidate in any red cells
            std::vector<int> collisions;
            if (collisionsFound(red, collisions))
            {
                indexes.insert(indexes.end(), red.begin(), red.end());
                values.push_back(v);
                reason = generateReason(v, collisions, red);
                return true;
            }

            // If any green cell can see any other green cell, then the value cannot be a candidate in any green cells
            if (collisionsFound(green, collisions))
            {
                indexes.insert(indexes.end(), green.begin(), green.end());
                values.push_back(v);
                reason = generateReason(v, collisions, green);
                return true;
            }

            // If any cell with a candidate v not in the chain can see both a red cell and a green cell, then the cell cannot have v
            // as a candidate
            int other;
            std::vector<int> redDependents;
            std::vector<int> greenDependents;
            if (canSeeBoth(v, red, green, other, redDependents, greenDependents))
            {
                indexes.push_back(other);
                values.push_back(v);
                reason = generateReason(v, other, redDependents, greenDependents);
                return true;
            }
        }
    }
    return false;
}

std::string SimpleColoring::generateReason(int v, std::vector<int> const & collisions, std::set<int> const & eliminated)
{
    std::string reason = "These squares cannot be ";
    reason += std::to_string(v);
    reason += " because if any of them were, it would lead to a contradiction in which";
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

void SimpleColoring::createGraph(int v, int i0, std::set<int> & a, std::set<int> & b)
{
    // From cell i0, recursively build a DAG of strong links between unlinked cells with candidate v.
    // This cell goes into list 'a'. The ones linked to it go into list 'b'.
    a.insert(i0);
    Link::Strong::List links = Link::Strong::find(candidates_, i0);
    for (auto const & link : links)
    {
        // Only care about this value for simple coloring
        if (link.v0 != v || link.v1 != v)
            continue;

        // If the other cell hasn't already been included in list 'b', then recursively extend the graph. Note that there is
        // no possibility of the other cell already being in list 'a'.
        int other = (i0 == link.i0) ? link.i1 : link.i0;
        if (b.find(other) == b.end())
            createGraph(v, other, b, a);  // Note: swapping the lists in order to alternate inferences.
    }
}

bool SimpleColoring::collisionsFound(std::set<int> const & indexes, std::vector<int> & collisions)
{
    if (!indexes.empty())
    {
        auto pBack = std::prev(indexes.end());
        for (auto pI = indexes.begin(); pI != pBack; ++pI)
        {
            std::vector<int> const & seen = Board::Cell::dependents(*pI);
            std::set_intersection(seen.begin(), seen.end(), std::next(pI), indexes.end(), std::back_inserter(collisions));
            if (!collisions.empty())
            {
                collisions.insert(collisions.begin(), *pI);
                return true;
            }
        }
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
                if (redDependents.empty())
                    continue;

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
    return false;
}
