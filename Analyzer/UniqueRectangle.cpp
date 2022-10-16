#include "UniqueRectangle.h"

#include "Board/Board.h"
#include "Candidates.h"

#include <string>
#include <utility>
#include <vector>

// A Pair is a pair of indexes (in the same row or column)
using Pair = std::pair<int, int>;

// A Rect is a pair of pairs. The first pair is the floor indexes, and the second is the roof indexes.
using Rect = std::pair<Pair, Pair>;

bool UniqueRectangle::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // If 4 cells containing the same exclusive pairs are within 2 boxes and form a rectangle, then the
    // solution cannot be unique. Therefore, any candidates that lead to this condition can be removed.

    // Find all exclusive pairs whose cells are in the same box and row or column (the "floors").
    // Note that types 2B, 2C, 3, 3B, 3/3B w/triples, and 4B are not supported when requiring the floor to be in a
    // single box.
    std::vector<Pair> floors;
    Board::ForEach::box([&](int b, std::vector<int> const & box) {
                            for (int i = 0; i < box.size() - 1; ++i)
                            {
                                int b0 = box[i];
                                if (Candidates::isBivalue(candidates_[b0]))
                                {
                                    for (int j = i + 1; j < box.size(); ++j)
                                    {
                                        int b1 = box[j];

                                        // If its the same as the other bivalue and it is in the same row or column, then we have a
                                        // floor.
                                        if (candidates_[b0] == candidates_[b1])
                                        {
                                            int r0, c0, r1, c1;
                                            Board::Cell::locationOf(b0, &r0, &c0);
                                            Board::Cell::locationOf(b1, &r1, &c1);
                                            if (r0 == r1 || c0 == c1)
                                                floors.push_back(std::make_pair(b0, b1));
                                        }
                                    }
                                }
                            }
                            return true;
                        });

    // Go through the floors and look for roofs.
    for (Pair floor : floors)
    {
        Candidates::Type floorCandidates = candidates_[floor.first];

        int r0, c0, r1, c1;
        Board::Cell::locationOf(floor.first, &r0, &c0);
        Board::Cell::locationOf(floor.second, &r1, &c1);

        // If the floor is in a row, look for the roof in the other rows. Otherwise, look in the other columns
        if (r0 == r1)
        {
            for (int r = 0; r < Board::SIZE; ++r)
            {
                if (r != r0)
                {
                    int roof0 = Board::Cell::indexOf(r, c0);
                    int roof1 = Board::Cell::indexOf(r, c1);
                    if (exists(floor.first, floor.second, roof0, roof1, floorCandidates, indexes, values, reason))
                        return true;
                }
            }
        }
        else
        {
            for (int c = 0; c < Board::SIZE; ++c)
            {
                if (c != c0)
                {
                    int roof0 = Board::Cell::indexOf(r0, c);
                    int roof1 = Board::Cell::indexOf(r1, c);
                    if (exists(floor.first, floor.second, roof0, roof1, floorCandidates, indexes, values, reason))
                        return true;
                }
            }
        }
    }
    return false;
}

bool UniqueRectangle::exists(int                floor0,
                             int                floor1,
                             int                roof0,
                             int                roof1,
                             Candidates::Type   floorCandidates,
                             std::vector<int> & indexes,
                             std::vector<int> & values,
                             std::string &      reason)
{
    bool corner3Matches            = candidates_[roof0] == floorCandidates;
    bool corner4Matches            = candidates_[roof1] == floorCandidates;
    bool corner3IncludesPair       = !corner3Matches && (candidates_[roof0] & floorCandidates) == floorCandidates;
    bool corner4IncludesPair       = !corner4Matches && (candidates_[roof1] & floorCandidates) == floorCandidates;
    Candidates::Type corner3Others = candidates_[roof0] & ~floorCandidates;
    Candidates::Type corner4Others = candidates_[roof1] & ~floorCandidates;

    // Type 1: Exactly one of the corners of the roof has other candidates and can only be those
    // other candidates
    if (corner3Matches && corner4IncludesPair || corner3IncludesPair && corner4Matches)
    {
        if (corner3IncludesPair)
            indexes.push_back(roof0);
        else
            indexes.push_back(roof1);
        values = Candidates::values(floorCandidates);
        reason = generateType1Reason(floor0, floor1, roof0, roof1, indexes[0], values);
        return true;
    }

    // Type 2: Both corners of the roof have exactly one other candidate and that candidate can
    // be eliminated from any cells that can see both corners of the roof.
    else if (corner3IncludesPair &&
             corner4IncludesPair &&
             corner3Others == corner4Others &&
             Candidates::isSolved(corner3Others))
    {
        std::vector<int> dependents = Board::Cell::dependents(roof0, roof1);
        for (int d : dependents)
        {
            if (candidates_[d] & corner3Others)
                indexes.push_back(d);
        }
        if (!indexes.empty())
        {
            values = Candidates::values(corner3Others);
            reason = generateType2Reason(floor0, floor1, roof0, roof1, indexes, values[0]);
            return true;
        }
    }
    return false;
}

std::string UniqueRectangle::generateType1Reason(int                      floor0,
                                                 int                      floor1,
                                                 int                      roof0,
                                                 int                      roof1,
                                                 int                      index,
                                                 std::vector<int> const & values)
{
    std::string reason;
    reason = "Square " +
             Board::Cell::name(index) +
             " cannot have the values " +
             std::to_string(values[0]) +
             " or " +
             std::to_string(values[1]) +
             " because having the same pairs at " +
             Board::Cell::name(floor0) + " " +
             Board::Cell::name(floor1) + " " +
             Board::Cell::name(roof0) + " " +
             Board::Cell::name(roof1) +
             " would result in a non-unique solution.";
    return reason;
}
std::string UniqueRectangle::generateType2Reason(int                      floor0,
                                                 int                      floor1,
                                                 int                      roof0,
                                                 int                      roof1,
                                                 std::vector<int> const & indexes,
                                                 int                      value)
{
    std::string reason;
    reason = "One of " +
             Board::Cell::name(roof0) +
             " or " +
             Board::Cell::name(roof1) +
             " must have the value " +
             std::to_string(value) +
             " because having the same pairs at " +
             Board::Cell::name(floor0) + " " +
             Board::Cell::name(floor1) + " " +
             Board::Cell::name(roof0) + " " +
             Board::Cell::name(roof1) +
             " would result in a non-unique solution. So, ";
    for (int i : indexes)
    {
        reason += Board::Cell::name(i);
        reason += " ";
    }
    reason += "cannot have the value ";
    reason += std::to_string(value);
    reason += ".";
    return reason;
}
