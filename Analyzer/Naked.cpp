#include "Naked.h"

#include "Candidates.h"

#include "Board/Board.h"

#include <vector>
#include <string>

bool Naked::singleExists(Board const & board,
    Candidates::List const & candidates,
    std::vector<int> & indexes,
    std::vector<int> & values,
    std::string & reason)
{
    // For each unsolved cell, if it only has one candidate, then success

    if (Naked::single(board, candidates, indexes, values))
    {
        reason = "There are no other possible values for this square.";
        return true;
    }
    return false;
}

bool Naked::pairExists(Board const & board,
    Candidates::List const & candidates,
    std::vector<int> & indexes,
    std::vector<int> & values,
    std::string & reason)
{
    // For each exclusive pair in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (Naked::pair(candidates, row, indexes, values, nakedIndexes))
        {
            which = r;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = pairReason("row", Board::Unit::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (Naked::pair(candidates, column, indexes, values, nakedIndexes))
        {
            which = c;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = pairReason("column", Board::Unit::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (Naked::pair(candidates, box, indexes, values, nakedIndexes))
        {
            which = b;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = pairReason("box", Board::Unit::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Naked::tripleExists(Board const & board,
    Candidates::List const & candidates,
    std::vector<int> & indexes,
    std::vector<int> & values,
    std::string & reason)
{
    // For each exclusive triple in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (Naked::triple(candidates, row, indexes, values, nakedIndexes))
        {
            which = r;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = tripleReason("row", Board::Unit::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (Naked::triple(candidates, column, indexes, values, nakedIndexes))
        {
            which = c;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = tripleReason("column", Board::Unit::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (Naked::triple(candidates, box, indexes, values, nakedIndexes))
        {
            which = b;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = tripleReason("box", Board::Unit::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Naked::quadExists(Board const & board,
    Candidates::List const & candidates,
    std::vector<int> & indexes,
    std::vector<int> & values,
    std::string & reason)
{
    // For each exclusive quad in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (Naked::quad(candidates, row, indexes, values, nakedIndexes))
        {
            which = r;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = quadReason("row", Board::Unit::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (Naked::quad(candidates, column, indexes, values, nakedIndexes))
        {
            which = c;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = quadReason("column", Board::Unit::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (Naked::quad(candidates, box, indexes, values, nakedIndexes))
        {
            which = b;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        reason = quadReason("box", Board::Unit::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Naked::single(Board const & board,
    Candidates::List const & candidates,
    std::vector<int> & indexes,
    std::vector<int> & values)
{
    return !Board::ForEach::cell([&](int i) {
        if (board.isEmpty(i))
        {
            Candidates::Type c = candidates[i];
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

bool Naked::pair(Candidates::List const & candidates,
    std::vector<int> const & indexes,
    std::vector<int> &       eliminatedIndexes,
    std::vector<int> &       eliminatedValues,
    std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 1; ++b0)
    {
        int i0 = indexes[b0];
        Candidates::Type candidates0 = candidates[i0];
        Candidates::Type cumulativeCandidates0 = candidates0;
        int count0 = Candidates::count(candidates0);
        if (count0 > 1 && count0 <= 2)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE; ++b1)
            {
                int i1 = indexes[b1];
                Candidates::Type c = candidates[i1];
                Candidates::Type cumulativeCandidates = cumulativeCandidates0 | c;
                if (Candidates::count(c) > 1 && Candidates::count(cumulativeCandidates) == 2)
                {
                    Board::ForEach::indexExcept(indexes, i0, i1, [&](int i) {
                        if (candidates[i] & cumulativeCandidates)
                            eliminatedIndexes.push_back(i);
                    });
                    if (!eliminatedIndexes.empty())
                    {
                        eliminatedValues = Candidates::values(cumulativeCandidates);
                        nakedIndexes = { i0, i1 };
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Naked::triple(Candidates::List const & candidates,
    std::vector<int> const & indexes,
    std::vector<int> &       eliminatedIndexes,
    std::vector<int> &       eliminatedValues,
    std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 2; ++b0)
    {
        int i0 = indexes[b0];
        Candidates::Type candidates0 = candidates[i0];
        Candidates::Type cumulativeCandidates0 = candidates0;
        int count0 = Candidates::count(candidates0);
        if (count0 > 1 && count0 <= 3)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 1; ++b1)
            {
                int i1 = indexes[b1];
                Candidates::Type candidates1 = candidates[i1];
                Candidates::Type cumulativeCandidates1 = cumulativeCandidates0 | candidates1;
                if (Candidates::count(candidates1) > 1 && Candidates::count(cumulativeCandidates1) <= 3)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE; ++b2)
                    {
                        int i2 = indexes[b2];
                        Candidates::Type c = candidates[i2];
                        Candidates::Type cumulativeCandidates = cumulativeCandidates1 | c;
                        if (Candidates::count(c) > 1 && Candidates::count(cumulativeCandidates) == 3)
                        {
                            Board::ForEach::indexExcept(indexes, i0, i1, i2, [&](int i) {
                                if (candidates[i] & cumulativeCandidates)
                                    eliminatedIndexes.push_back(i);
                            });
                            if (!eliminatedIndexes.empty())
                            {
                                eliminatedValues = Candidates::values(cumulativeCandidates);
                                nakedIndexes = { i0, i1, i2 };
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

bool Naked::quad(Candidates::List const & candidates,
    std::vector<int> const & indexes,
    std::vector<int> &       eliminatedIndexes,
    std::vector<int> &       eliminatedValues,
    std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 3; ++b0)
    {
        int i0 = indexes[b0];
        Candidates::Type candidates0 = candidates[i0];
        Candidates::Type cumulativeCandidates0 = candidates0;
        int count0 = Candidates::count(candidates0);
        if (count0 > 1 && count0 <= 4)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 2; ++b1)
            {
                int i1 = indexes[b1];
                Candidates::Type candidates1 = candidates[i1];
                Candidates::Type cumulativeCandidates1 = cumulativeCandidates0 | candidates1;
                if (Candidates::count(candidates1) > 1 && Candidates::count(cumulativeCandidates1) <= 4)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE - 1; ++b2)
                    {
                        int i2 = indexes[b2];
                        Candidates::Type candidates2 = candidates[i2];
                        Candidates::Type cumulativeCandidates2 = cumulativeCandidates1 | candidates2;
                        if (Candidates::count(candidates2) > 1 && Candidates::count(cumulativeCandidates2) <= 4)
                        {
                            for (int b3 = b2 + 1; b3 < Board::SIZE; ++b3)
                            {
                                int i3 = indexes[b3];
                                Candidates::Type c = candidates[i3];
                                Candidates::Type cumulativeCandidates = cumulativeCandidates2 | c;
                                if (Candidates::count(c) > 1 && Candidates::count(cumulativeCandidates) == 4)
                                {
                                    Board::ForEach::indexExcept(indexes, i0, i1, i2, i3, [&](int i) {
                                        if (candidates[i] & cumulativeCandidates)
                                            eliminatedIndexes.push_back(i);
                                    });
                                    if (!eliminatedIndexes.empty())
                                    {
                                        eliminatedValues = Candidates::values(cumulativeCandidates);
                                        nakedIndexes = { i0, i1, i2, i3 };
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

std::string Naked::pairReason(std::string const & unit, char which, std::vector<int> const & indexes)
{
    std::string reason = "Two other squares (" +
        Board::Cell::name(indexes[0]) +
        " and " +
        Board::Cell::name(indexes[1]) +
        ") in " +
        unit +
        " " +
        which +
        " must be one of these two values, so these squares cannot be either of these two values.";
    return reason;
}

std::string Naked::tripleReason(std::string const & unit, char which, std::vector<int> const & indexes)
{
    std::string reason = std::string("Three other squares (") +
        Board::Cell::name(indexes[0]) +
        ", " +
        Board::Cell::name(indexes[1]) +
        " and " +
        Board::Cell::name(indexes[2]) +
        ") in " +
        unit + " " + which +
        " must be one of these three values, so these squares cannot be any of these three values.";
    return reason;
}

std::string Naked::quadReason(std::string const & unit, char which, std::vector<int> const & indexes)
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
        unit +
        " " +
        which +
        " must be one of these four values, so these squares cannot be any of these four values.";
    return reason;
}
