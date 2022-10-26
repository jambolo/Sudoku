#include "XCycle.h"

#include "Board/Board.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include <vector>

bool XCycle::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason, bool & solves)
{
    // For each candidate in each cell:
    //  Form a graph of alternating strong and weak links of the candidate value rooted at the cell starting with a strong link
    //  until a cycle is found.
    //  1. If the last link in the cycle is a weak link, then any cells that can see both ends of any weak link in the cycle cannot
    // have the candidates value.
    //  2. If the last link in the cycle is a strong link, then the first cell of cycle must be the candidate value.
    //  3. Any cell that can see root cell and the end of a strong link in an incomplete cycle cannot have the candidate value.

    for (int v = 1; v <= 9; ++v)
    {
        // Save the value
        value_ = v;

        // Find any x-cycles for the value
        for (int r = 0; r < Board::NUM_CELLS; ++r)
        {
            if (Candidates::isSolved(candidates_[r]) || !Candidates::includes(candidates_[r], v))
                continue;

            // Save the root index
            root_ = r;

            // The first link is a strong link. If a useful x-cycle was found, return the success
            if (extendStrong(r))
            {
                indexes = indexes_;
                values  = values_;
                reason  = reason_;
                solves  = solves_;
                return true;
            }
        }
    }
    return false;
}

bool XCycle::extendAny(int from)
{
    // Extend weak links
    {
        // Find all weak links at this index
        Link::Weak::List links = Link::Weak::find(candidates_, from, value_);

        // Remove any links to cells that we have already visited
        links.erase(std::remove_if(links.begin(), links.end(), [from, this](Link::Weak const & a) {
                                       int linkEnd = (a.i0 == from) ? a.i1 : a.i0;
                                       return std::find(chain_.begin(), chain_.end(), linkEnd) != chain_.end();
                                   }),
                    links.end());

        // For each weak link, check for the completion of a cycle and if not then try to extend the graph with a strong link
        for (auto const & link : links)
        {
            weakLinks_.push_back(link);
            int newEnd = (from == link.i0) ? link.i1 : link.i0;

            // If the end of this link is the root cell, then we have a cycle.
            if (newEnd == root_)
            {
                // Any cells with the candidate that can see both ends of any weak link in the cycle cannot have the candidate's
                // value.
                std::vector<Reason1Dependency> reason1Dependencies;

                for (auto const & w : weakLinks_)
                {
                    std::vector<int> dependents = Board::Cell::dependents(w.i0, w.i1);
                    for (auto i : dependents)
                    {
                        if (Candidates::includes(candidates_[i], value_))
                        {
                            indexes_.push_back(i);
                            reason1Dependencies.emplace_back(Reason1Dependency{ i, w.i0, w.i1 });
                        }
                    }
                }

                // If any cells can be eliminated, then success
                if (!indexes_.empty())
                {
                    // Remove duplicate cells
                    std::sort(indexes_.begin(), indexes_.end());
                    indexes_.erase(std::unique(indexes_.begin(), indexes_.end()), indexes_.end());

                    values_.push_back(value_);
                    reason_ = generateReason1(reason1Dependencies);
                    solves_ = false;
                    return true;
                }
            }
            // Otherwise, extend the graph with a strong link from the end of this weak link
            else
            {
                // Add this index to the list of visited indexes in order to avoid loops
                chain_.push_back(newEnd);

                if (extendStrong(newEnd))
                    return true;

                // This index is no longer part of the chain so remove it from the list of visited indexes
                chain_.pop_back();
            }

            // Remove this weak link from the list since it is no longer in the chain
            weakLinks_.pop_back();
        }
    }

    // Extend strong links
    {
        // Find all strong links at this index
        Link::Strong::List links = Link::Strong::find(candidates_, from, value_);

        // Remove any links to cells that we have already visited
        links.erase(std::remove_if(links.begin(), links.end(), [from, this](Link::Strong const & a) {
                                       int linkEnd = (a.i0 == from) ? a.i1 : a.i0;
                                       return std::find(chain_.begin(), chain_.end(), linkEnd) != chain_.end();
                                   }),
                    links.end());

        // For each strong link, check for the completion of a cycle and if not then try to extend the graph with a strong link
        for (auto const & link : links)
        {
            int newEnd = (from == link.i0) ? link.i1 : link.i0;

            // If the end of this link is the root cell, then we have a cycle.
            if (newEnd == root_)
            {
                // Any cells with the candidate that can see both ends of any weak link in the cycle cannot have the candidate's
                // value.
                std::vector<Reason1Dependency> reason1Dependencies;

                for (auto const & w : weakLinks_)
                {
                    std::vector<int> dependents = Board::Cell::dependents(w.i0, w.i1);
                    for (auto i : dependents)
                    {
                        if (Candidates::includes(candidates_[i], value_))
                        {
                            indexes_.push_back(i);
                            reason1Dependencies.emplace_back(Reason1Dependency{ i, w.i0, w.i1 });
                        }
                    }
                }

                // If any cells can be eliminated, then success
                if (!indexes_.empty())
                {
                    // Remove duplicate cells
                    std::sort(indexes_.begin(), indexes_.end());
                    indexes_.erase(std::unique(indexes_.begin(), indexes_.end()), indexes_.end());

                    values_.push_back(value_);
                    reason_ = generateReason1(reason1Dependencies);
                    solves_ = false;
                    return true;
                }
            }
            // Otherwise, extend the graph with a strong link from the end of this link
            else
            {
                // Add this index to the list of visited indexes in order to avoid loops
                chain_.push_back(newEnd);

                if (extendStrong(newEnd))
                    return true;

                // This index is no longer part of the chain so remove it from the list of visited indexes
                chain_.pop_back();
            }
        }
    }

    // All links were tried and nothing was found
    return false;
}

// Recursively form a graph of alternating weak and strong links by extending each strong link from end
bool XCycle::extendStrong(int from)
{
    // Find all strong links at this index
    Link::Strong::List links = Link::Strong::find(candidates_, from, value_);

    // Remove any links to cells that we have already visited
    links.erase(std::remove_if(links.begin(), links.end(), [from, this](Link::Strong const & a) {
                                   int linkEnd = (a.i0 == from) ? a.i1 : a.i0;
                                   return std::find(chain_.begin(), chain_.end(), linkEnd) != chain_.end();
                               }),
                links.end());

    // For each strong link, check for any hits and if not then try to extend the graph with a weak link
    for (auto const & link : links)
    {
        int newEnd = (from == link.i0) ? link.i1 : link.i0;

        // If end of this link is the root, then the root must have the candidate value.
        if (newEnd == root_)
        {
            indexes_.push_back(root_);
            values_.push_back(value_);
            reason_ = generateReason2(chain_.front(), from);
            solves_ = true;
            return true;
        }
        else
        {
            // Add this index to the list of visited indexes in order to avoid loops
            chain_.push_back(newEnd);

            // Extend the chain with any links from this end.
            bool solved = extendAny(newEnd);

            // This index is no longer part of the chain so remove it from the list of visited indexes
            chain_.pop_back();

            // If a cycle is found, then return
            if (solved)
            {
                return true;
            }
            // Otherwise, there is no cycle but any cell that can see root cell and the end of this strong link cannot have the
            // candidate value.
            else
            {
                std::vector<int> dependents = Board::Cell::dependents(root_, newEnd);
                for (auto i : dependents)
                {
                    if (Candidates::includes(candidates_[i], value_))
                        indexes_.push_back(i);
                }
                if (!indexes_.empty())
                {
                    values_.push_back(value_);
                    reason_ = generateReason3(newEnd);
                    solves_ = false;
                    return true;
                }
            }
        }
    }

    // All links were tried and nothing was found
    return false;
}

std::string XCycle::generateReason1(std::vector<Reason1Dependency> const & dependencies)
{
    std::string reason;

    for (int i : indexes_)
    {
        reason += Board::Cell::name(i);
        reason += " ";
    }
    reason += "cannot have the value " +
              std::to_string(values_[0]) +
              " because one square in each of the pairs ";

    // Get the weak link indexes that the eliminated cells see and remove duplicates
    std::set<std::pair<int, int>> links;
    for (auto i : indexes_)
    {
        auto d = std::find_if(dependencies.begin(), dependencies.end(), [i](Reason1Dependency const & a) {
                                  return a.dependent == i;
                              });
        links.insert(std::make_pair(d->weak0, d->weak1));
    }

    for (auto const & link : links)
    {
        reason += "(" + Board::Cell::name(link.first) + "," + Board::Cell::name(link.second) + ") ";
    }
    reason += "must have that value.";
    return reason;
}

std::string XCycle::generateReason2(int second, int last)
{
    return Board::Cell::name(root_) +
           " must be " +
           std::to_string(value_) +
           ". Otherwise, " +
           Board::Cell::name(second) +
           " and " +
           Board::Cell::name(last) +
           " would have to be that value and that would cause a contradiction.";
}

std::string XCycle::generateReason3(int last)
{
    std::string reason;
    for (int i : indexes_)
    {
        reason += Board::Cell::name(i);
        reason += " ";
    }
    reason += "cannot have the value " +
              std::to_string(value_) +
              " because one of " +
              Board::Cell::name(root_) +
              " " +
              Board::Cell::name(last) +
              " must have that value.";
    return reason;
}
