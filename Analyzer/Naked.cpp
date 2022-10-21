#include "Naked.h"

#include "Candidates.h"

#include "Board/Board.h"

#include <string>
#include <vector>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

bool Naked::singleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each unsolved cell, if it only has one candidate, then success

    if (Naked::single(indexes, values))
    {
        reason = "There are no other possible values for this square.";
        return true;
    }
    return false;
}

bool Naked::pairExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive pair in a group, if there are other candidates that overlap, then success.

    bool found;
    int  which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
                                     if (Naked::pair(row, indexes, values, nakedIndexes))
                                     {
                                         which = r;
                                         return false; // done
                                     }
                                     return true;
                                 });
    if (found)
    {
        reason = pairReason("row", Board::Group::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
                                        if (Naked::pair(column, indexes, values, nakedIndexes))
                                        {
                                            which = c;
                                            return false; // done
                                        }
                                        return true;
                                    });
    if (found)
    {
        reason = pairReason("column", Board::Group::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
                                     if (Naked::pair(box, indexes, values, nakedIndexes))
                                     {
                                         which = b;
                                         return false; // done
                                     }
                                     return true;
                                 });
    if (found)
    {
        reason = pairReason("box", Board::Group::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Naked::tripleExists(std::vector<int> & indexes,
                         std::vector<int> & values,
                         std::string &      reason)
{
    // For each exclusive triple in a group, if there are other candidates that overlap, then success.

    bool found;
    int  which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
                                     if (Naked::triple(row, indexes, values, nakedIndexes))
                                     {
                                         which = r;
                                         return false; // done
                                     }
                                     return true;
                                 });
    if (found)
    {
        reason = tripleReason("row", Board::Group::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
                                        if (Naked::triple(column, indexes, values, nakedIndexes))
                                        {
                                            which = c;
                                            return false; // done
                                        }
                                        return true;
                                    });
    if (found)
    {
        reason = tripleReason("column", Board::Group::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
                                     if (Naked::triple(box, indexes, values, nakedIndexes))
                                     {
                                         which = b;
                                         return false; // done
                                     }
                                     return true;
                                 });
    if (found)
    {
        reason = tripleReason("box", Board::Group::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Naked::quadExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive quad in a group, if there are other candidates that overlap, then success.

    bool found;
    int  which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
                                     if (Naked::quad(row, indexes, values, nakedIndexes))
                                     {
                                         which = r;
                                         return false; // done
                                     }
                                     return true;
                                 });
    if (found)
    {
        reason = quadReason("row", Board::Group::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
                                        if (Naked::quad(column, indexes, values, nakedIndexes))
                                        {
                                            which = c;
                                            return false; // done
                                        }
                                        return true;
                                    });
    if (found)
    {
        reason = quadReason("column", Board::Group::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
                                     if (Naked::quad(box, indexes, values, nakedIndexes))
                                     {
                                         which = b;
                                         return false; // done
                                     }
                                     return true;
                                 });
    if (found)
    {
        reason = quadReason("box", Board::Group::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Naked::single(std::vector<int> & nakedIndexes, std::vector<int> & nakedValues)
{
    return !Board::ForEach::cell([&](int i) {
                                     if (board_.isEmpty(i))
                                     {
                                         Candidates::Type c = candidates_[i];
                                         if (Candidates::isSolved(c))
                                         {
                                             nakedIndexes.push_back(i);
                                             nakedValues.push_back(Candidates::value(c));
                                             return false;
                                         }
                                     }
                                     return true;
                                 });
}

bool Naked::pair(std::vector<int> const & group,
                 std::vector<int> &       eliminatedIndexes,
                 std::vector<int> &       eliminatedValues,
                 std::vector<int> &       nakedIndexes)
{
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    for (int b0 = 0; b0 < Board::SIZE - 1; ++b0)
    {
        int i0 = group[b0];
        Candidates::Type candidates0 = candidates_[i0];
        if (Candidates::isSolved(candidates0) || Candidates::count(candidates0) > 2)
            continue;
        for (int b1 = b0 + 1; b1 < Board::SIZE; ++b1)
        {
            int i1 = group[b1];
            Candidates::Type candidates1 = candidates_[i1];
            if (Candidates::isSolved(candidates1))
                continue;
            // If these two cells form a conjugate pair, then eliminate their candidates from other cells in the group
            Candidates::Type combined = candidates0 | candidates1;
            if (Candidates::count(combined) == 2)
            {
                for (int i : group)
                {
                    if (i != i0 && i != i1 && (candidates_[i] & combined) != Candidates::NONE)
                        eliminatedIndexes.push_back(i);
                }

                // If any candidates were eliminated, then success
                if (!eliminatedIndexes.empty())
                {
                    eliminatedValues = Candidates::values(combined);
                    nakedIndexes     = { i0, i1 };
                    return true;
                }
            }
        }
    }
    return false;
}

bool Naked::triple(std::vector<int> const & group,
                   std::vector<int> &       eliminatedIndexes,
                   std::vector<int> &       eliminatedValues,
                   std::vector<int> &       nakedIndexes)
{
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    for (int b0 = 0; b0 < Board::SIZE - 2; ++b0)
    {
        int i0 = group[b0];
        Candidates::Type candidates0 = candidates_[i0];
        if (Candidates::isSolved(candidates0) || Candidates::count(candidates0) > 3)
            continue;
        for (int b1 = b0 + 1; b1 < Board::SIZE - 1; ++b1)
        {
            int i1 = group[b1];
            Candidates::Type candidates1 = candidates_[i1];
            if (Candidates::isSolved(candidates1) || Candidates::count(candidates0 | candidates1) > 3)
                continue;
            for (int b2 = b1 + 1; b2 < Board::SIZE; ++b2)
            {
                int i2 = group[b2];
                Candidates::Type candidates2 = candidates_[i2];
                if (Candidates::isSolved(candidates2))
                    continue;
                // If these three cells form a conjugate triple, then eliminate their candidates from other cells in the group
                Candidates::Type combined = candidates0 | candidates1 | candidates2;
                if (Candidates::count(combined) == 3)
                {
                    for (int i : group)
                    {
                        if (i != i0 && i != i1 && i != i2 && (candidates_[i] & combined) != Candidates::NONE)
                            eliminatedIndexes.push_back(i);
                    }

                    // If any candidates were eliminated, then success
                    if (!eliminatedIndexes.empty())
                    {
                        eliminatedValues = Candidates::values(combined);
                        nakedIndexes     = { i0, i1, i2 };
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Naked::quad(std::vector<int> const & group,
                 std::vector<int> &       eliminatedIndexes,
                 std::vector<int> &       eliminatedValues,
                 std::vector<int> &       nakedIndexes)
{
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    for (int b0 = 0; b0 < Board::SIZE - 3; ++b0)
    {
        int i0 = group[b0];
        Candidates::Type candidates0 = candidates_[i0];
        if (Candidates::isSolved(candidates0) || Candidates::count(candidates0) > 4)
            continue;
        for (int b1 = b0 + 1; b1 < Board::SIZE - 2; ++b1)
        {
            int i1 = group[b1];
            Candidates::Type candidates1 = candidates_[i1];
            if (Candidates::isSolved(candidates1) || Candidates::count(candidates0 | candidates1) > 4)
                continue;
            for (int b2 = b1 + 1; b2 < Board::SIZE - 1; ++b2)
            {
                int i2 = group[b2];
                Candidates::Type candidates2 = candidates_[i2];
                if (Candidates::isSolved(candidates2) || Candidates::count(candidates0 | candidates1 | candidates2) > 4)
                    continue;
                for (int b3 = b2 + 1; b3 < Board::SIZE; ++b3)
                {
                    int i3 = group[b3];
                    Candidates::Type candidates3 = candidates_[i3];
                    if (Candidates::isSolved(candidates3))
                        continue;
                    // If these four cells form a conjugate quad, then eliminate their candidates from other cells in the group
                    Candidates::Type combined = candidates0 | candidates1 | candidates2 | candidates3;
                    if (Candidates::count(combined) == 4)
                    {
                        for (int i : group)
                        {
                            if (i != i0 && i != i1 && i != i2 && i != i3 &&
                                (candidates_[i] & combined) != Candidates::NONE)
                            {
                                eliminatedIndexes.push_back(i);
                            }
                        }
                        if (!eliminatedIndexes.empty())
                        {
                            eliminatedValues = Candidates::values(combined);
                            nakedIndexes     = { i0, i1, i2, i3 };
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

std::string Naked::pairReason(std::string const & group, char which, std::vector<int> const & indexes)
{
    std::string reason = "Two other squares (" +
                         Board::Cell::name(indexes[0]) +
                         " and " +
                         Board::Cell::name(indexes[1]) +
                         ") in " +
                         group +
                         " " +
                         which +
                         " must be one of these two values, so these squares cannot be either of these two values.";
    return reason;
}

std::string Naked::tripleReason(std::string const & group, char which, std::vector<int> const & indexes)
{
    std::string reason = std::string("Three other squares (") +
                         Board::Cell::name(indexes[0]) +
                         ", " +
                         Board::Cell::name(indexes[1]) +
                         " and " +
                         Board::Cell::name(indexes[2]) +
                         ") in " +
                         group + " " + which +
                         " must be one of these three values, so these squares cannot be any of these three values.";
    return reason;
}

std::string Naked::quadReason(std::string const & group, char which, std::vector<int> const & indexes)
{
    std::string reason = "Four other squares (" +
                         Board::Cell::name(indexes[0]) +
                         ", " +
                         Board::Cell::name(indexes[1]) +
                         ", " +
                         Board::Cell::name(indexes[2]) +
                         " and " +
                         Board::Cell::name(indexes[3]) +
                         ") in " +
                         group +
                         " " +
                         which +
                         " must be one of these four values, so these squares cannot be any of these four values.";
    return reason;
}
