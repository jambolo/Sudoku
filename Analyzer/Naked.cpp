#include "Naked.h"

#include "Candidates.h"

#include "Board/Board.h"

#include <string>
#include <vector>

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
    int which;
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
    int which;
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
    int which;
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

bool Naked::single(std::vector<int> & indexes, std::vector<int> & values)
{
    return !Board::ForEach::cell([&](int i) {
        if (board_.isEmpty(i))
        {
            Candidates::Type c = candidates_[i];
            if (Candidates::solved(c))
            {
                indexes.push_back(i);
                values.push_back(Candidates::value(c));
                return false;
            }
        }
        return true;
    });
}

bool Naked::pair(std::vector<int> const & indexes,
                 std::vector<int> &       eliminatedIndexes,
                 std::vector<int> &       eliminatedValues,
                 std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 1; ++b0)
    {
        int i0 = indexes[b0];
        Candidates::Type candidates0 = candidates_[i0];
        Candidates::Type cumulativeCandidates0 = candidates0;
        int count0 = Candidates::count(candidates0);
        if (count0 > 1 && count0 <= 2)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE; ++b1)
            {
                int i1 = indexes[b1];
                Candidates::Type c = candidates_[i1];
                Candidates::Type cumulativeCandidates = cumulativeCandidates0 | c;
                if (Candidates::count(c) > 1 && Candidates::count(cumulativeCandidates) == 2)
                {
                    Board::ForEach::indexExcept(indexes, i0, i1, [&](int i) {
                        if (candidates_[i] & cumulativeCandidates)
                            eliminatedIndexes.push_back(i);
                    });
                    if (!eliminatedIndexes.empty())
                    {
                        eliminatedValues = Candidates::values(cumulativeCandidates);
                        nakedIndexes     = { i0, i1 };
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Naked::triple(std::vector<int> const & indexes,
                   std::vector<int> &       eliminatedIndexes,
                   std::vector<int> &       eliminatedValues,
                   std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 2; ++b0)
    {
        int i0 = indexes[b0];
        Candidates::Type candidates0 = candidates_[i0];
        Candidates::Type cumulativeCandidates0 = candidates0;
        int count0 = Candidates::count(candidates0);
        if (count0 > 1 && count0 <= 3)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 1; ++b1)
            {
                int i1 = indexes[b1];
                Candidates::Type candidates1 = candidates_[i1];
                Candidates::Type cumulativeCandidates1 = cumulativeCandidates0 | candidates1;
                if (Candidates::count(candidates1) > 1 && Candidates::count(cumulativeCandidates1) <= 3)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE; ++b2)
                    {
                        int i2 = indexes[b2];
                        Candidates::Type c = candidates_[i2];
                        Candidates::Type cumulativeCandidates = cumulativeCandidates1 | c;
                        if (Candidates::count(c) > 1 && Candidates::count(cumulativeCandidates) == 3)
                        {
                            Board::ForEach::indexExcept(indexes, i0, i1, i2, [&](int i) {
                                if (candidates_[i] & cumulativeCandidates)
                                    eliminatedIndexes.push_back(i);
                            });
                            if (!eliminatedIndexes.empty())
                            {
                                eliminatedValues = Candidates::values(cumulativeCandidates);
                                nakedIndexes     = { i0, i1, i2 };
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Naked::quad(std::vector<int> const & indexes,
                 std::vector<int> &       eliminatedIndexes,
                 std::vector<int> &       eliminatedValues,
                 std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 3; ++b0)
    {
        int i0 = indexes[b0];
        Candidates::Type candidates0 = candidates_[i0];
        Candidates::Type cumulativeCandidates0 = candidates0;
        int count0 = Candidates::count(candidates0);
        if (count0 > 1 && count0 <= 4)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 2; ++b1)
            {
                int i1 = indexes[b1];
                Candidates::Type candidates1 = candidates_[i1];
                Candidates::Type cumulativeCandidates1 = cumulativeCandidates0 | candidates1;
                if (Candidates::count(candidates1) > 1 && Candidates::count(cumulativeCandidates1) <= 4)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE - 1; ++b2)
                    {
                        int i2 = indexes[b2];
                        Candidates::Type candidates2 = candidates_[i2];
                        Candidates::Type cumulativeCandidates2 = cumulativeCandidates1 | candidates2;
                        if (Candidates::count(candidates2) > 1 && Candidates::count(cumulativeCandidates2) <= 4)
                        {
                            for (int b3 = b2 + 1; b3 < Board::SIZE; ++b3)
                            {
                                int i3 = indexes[b3];
                                Candidates::Type c = candidates_[i3];
                                Candidates::Type cumulativeCandidates = cumulativeCandidates2 | c;
                                if (Candidates::count(c) > 1 && Candidates::count(cumulativeCandidates) == 4)
                                {
                                    Board::ForEach::indexExcept(indexes, i0, i1, i2, i3, [&](int i) {
                                        if (candidates_[i] & cumulativeCandidates)
                                            eliminatedIndexes.push_back(i);
                                    });
                                    if (!eliminatedIndexes.empty())
                                    {
                                        eliminatedValues = Candidates::values(cumulativeCandidates);
                                        nakedIndexes     = { i0, i1, i2, i3 };
                                        return true;
                                    }
                                }
                            }
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
