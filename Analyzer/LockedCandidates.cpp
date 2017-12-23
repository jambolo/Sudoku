#include "LockedCandidates.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

std::string LockedCandidates::generateReason(std::string const & group1,
                                             char                which1,
                                             std::string const & group2,
                                             char                which2)
{
    std::string reason = "Since the portion of " +
                         group2 +
                         " " +
                         which2 +
                         " within " +
                         group1 +
                         " " +
                         which1 +
                         " must contain these values, they cannot be anywhere else in " +
                         group2 +
                         " " +
                         which2;
    return reason;
}

bool LockedCandidates::exists(std::vector<int> & indexes,
                              std::vector<int> & values,
                              std::string &      reason)
{
    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the row/column, then success if those candidates exist in the box.

    bool found;
    int which1, which2;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        for (int c = 0; c < Board::SIZE; c += Board::BOX_SIZE)
        {
            int b = Board::Group::whichBox(r, c);
            std::vector<int> box = Board::Group::box(b);
            if (find(row, box, indexes, values))
            {
                which1 = r;
                which2 = b;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        reason = generateReason("row", Board::Group::rowName(which1), "box", Board::Group::boxName(which2));
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        for (int r = 0; r < Board::SIZE; r += Board::BOX_SIZE)
        {
            int b = Board::Group::whichBox(r, c);
            std::vector<int> box = Board::Group::box(b);
            if (find(column, box, indexes, values))
            {
                which1 = c;
                which2 = b;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        reason = generateReason("column", Board::Group::columnName(which1), "box", Board::Group::boxName(which2));
        return true;
    }

    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the box, then success if those candidates exist in the row/column.

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        for (int c = 0; c < Board::SIZE; c += Board::BOX_SIZE)
        {
            int b = Board::Group::whichBox(r, c);
            std::vector<int> box = Board::Group::box(b);
            if (find(box, row, indexes, values))
            {
                which1 = b;
                which2 = r;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        reason = generateReason("box", Board::Group::boxName(which1), "row", Board::Group::rowName(which2));
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        for (int r = 0; r < Board::SIZE; r += Board::BOX_SIZE)
        {
            int b = Board::Group::whichBox(r, c);
            std::vector<int> box = Board::Group::box(b);
            if (find(box, column, indexes, values))
            {
                which1 = b;
                which2 = c;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        reason = generateReason("box", Board::Group::boxName(which1), "row", Board::Group::columnName(which2));
        return true;
    }

    return false;
}

bool LockedCandidates::find(std::vector<int> const & indexes1,
                            std::vector<int> const & indexes2,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues)
{
    // Indexes in the intersection
    std::vector<int> intersection;
    std::set_intersection(indexes1.begin(), indexes1.end(),
                          indexes2.begin(), indexes2.end(),
                          std::back_inserter(intersection));

    // Indexes not in the intersection
    std::vector<int> others1;
    std::set_difference(indexes1.begin(), indexes1.end(),
                        intersection.begin(), intersection.end(),
                        std::back_inserter(others1));
    std::vector<int> others2;
    std::set_difference(indexes2.begin(), indexes2.end(),
                        intersection.begin(), intersection.end(),
                        std::back_inserter(others2));

    // Candidates in the intersection
    Candidates::Type intersectionCandidates = allCandidates(intersection);

    // Candidates in set 1, not in intersection
    Candidates::Type otherCandidates1 = allCandidates(others1);

    // If any of the candidates in the intersection don't exist in the rest of set 1, then they are the ones to be
    // eliminated from set 2
    Candidates::Type unique1 = intersectionCandidates & ~otherCandidates1;
    if (unique1)
    {
        for (int i : others2)
        {
            if (candidates_[i] & unique1)
                eliminatedIndexes.push_back(i);
        }
        if (!eliminatedIndexes.empty())
        {
            std::vector<int> uniqueCandidates = Candidates::values(unique1);
            eliminatedValues.insert(eliminatedValues.end(), uniqueCandidates.begin(), uniqueCandidates.end());
        }

        std::sort(eliminatedIndexes.begin(), eliminatedIndexes.end());
        eliminatedIndexes.erase(std::unique(eliminatedIndexes.begin(),
                                            eliminatedIndexes.end()),
                                eliminatedIndexes.end());
        std::sort(eliminatedValues.begin(), eliminatedValues.end());
        eliminatedValues.erase(std::unique(eliminatedValues.begin(),
                                           eliminatedValues.end()),
                               eliminatedValues.end());
    }

    return !eliminatedIndexes.empty();
}

Candidates::Type LockedCandidates::allCandidates(std::vector<int> const & indexes)
{
    Candidates::Type all = 0;
    for (int i : indexes)
    {
        Candidates::Type c = candidates_[i];
        if (!Candidates::solved(c))
            all |= c;
    }
    return all;
}
