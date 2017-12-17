#include "Hidden.h"

#include "Candidates.h"

#include "Board/Board.h"

#include <cassert>

bool Hidden::singleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    bool found;
    int which;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (single(row, indexes, values))
        {
            which = r;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = std::string("This is the only square in row ") +
                 Board::Unit::rowName(which) +
                 " that can have this value.";
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (single(column, indexes, values))
        {
            which = c;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = std::string("This is the only square in column ") +
                 Board::Unit::columnName(which) +
                 " that can have this value.";
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (single(box, indexes, values))
        {
            which = b;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = std::string("This is the only square in box ") +
                 Board::Unit::boxName(which) +
                 " that can have this value.";
        return true;
    }

    return false;
}

bool Hidden::single(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    for (int s : indexes)
    {
        if (board_.isEmpty(s))
        {
            Candidates::Type others = 0;
            Board::ForEach::indexExcept(indexes, s, [&] (int i) {
                others |= candidates_[i];
            });

            Candidates::Type exclusive = candidates_[s] & ~others;
            if (exclusive)
            {
                assert(Candidates::solved(exclusive));
                eliminatedIndexes.push_back(s);
                eliminatedValues.push_back(Candidates::value(exclusive));
                return true;
            }
        }
    }
    return false;
}

static std::string generateHiddenPairReason(std::string const & unitType, char which, std::vector<int> const & hiddenValues)
{
    std::string reason = "Only these two squares in " + unitType + " " + which +
                         " can be " + std::to_string(hiddenValues[0]) +
                         " or " + std::to_string(hiddenValues[1]) +
                         ", so they cannot be any other values.";
    return reason;
}

bool Hidden::pairExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive pair in a unit, if they have additional candidates_, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        if (pair(row, indexes, values, hidden))
        {
            which = r;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenPairReason("row", Board::Unit::rowName(which), hidden);
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        if (pair(column, indexes, values, hidden))
        {
            which = c;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenPairReason("column", Board::Unit::columnName(which), hidden);
        return true;
    }

    found = !Board::ForEach::box([&](int b, std::vector<int> const & box) {
        if (pair(box, indexes, values, hidden))
        {
            which = b;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenPairReason("box", Board::Unit::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Hidden::pair(std::vector<int> const & indexes,
                  std::vector<int> &       eliminatedIndexes,
                  std::vector<int> &       eliminatedValues,
                  std::vector<int> &       hiddenValues)
{
    // Go through each possible pair of candidates_ and search for exactly two cells containing the one or more of a pair
    for (int x0 = 1; x0 <= Board::SIZE - 1; ++x0)
    {
        Candidates::Type m0 = Candidates::fromValue(x0);
        for (int x1 = x0 + 1; x1 <= Board::SIZE; ++x1)
        {
            Candidates::Type m1 = Candidates::fromValue(x1);
            Candidates::Type m  = m0 | m1;

            // Count the number of cells with either of these two candidates_
            int nFound    = 0;
            int found[2];
            int counts[2] = { 0 };
            for (int i : indexes)
            {
                Candidates::Type c = candidates_[i];
                if (c & m)
                {
                    // Count the number of found indexes
                    ++nFound;
                    if (nFound > 2)
                        break; // Give up, must be exactly 2
                    found[nFound - 1] = i;

                    // Count the number of times the candidates_ occur
                    if (c & m0)
                        ++counts[0];
                    if (c & m1)
                        ++counts[1];
                }
            }
            // If there are exactly 2 indexes, each candidate is seen at least twice, and there are other candidates_ to eliminate,
            // then success
            if (nFound == 2 && counts[0] >= 2 && counts[1] >= 2)
            {
                Candidates::Type eliminatedCandidates = (candidates_[found[0]] | candidates_[found[1]]) & ~m;
                if (eliminatedCandidates)
                {
                    eliminatedIndexes.assign(std::begin(found), std::end(found));
                    eliminatedValues = Candidates::values(eliminatedCandidates);
                    hiddenValues     = Candidates::values(m);
                    return true;
                }
            }
        }
    }

    return false;
}

static std::string generateHiddenTripleReason(std::string const & unitType, char which, std::vector<int> const & hiddenValues)
{
    std::string reason = "Only these three squares in " + unitType + " " + which +
                         " can be " + std::to_string(hiddenValues[0]) +
                         ", " + std::to_string(hiddenValues[1]) +
                         ", or " + std::to_string(hiddenValues[2]) +
                         ", so they cannot be any other values.";
    return reason;
}

bool Hidden::tripleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive triple in a unit, if they have additional candidates_, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        if (triple(row, indexes, values, hidden))
        {
            which = r;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenTripleReason("row", Board::Unit::rowName(which), hidden);
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        if (triple(column, indexes, values, hidden))
        {
            which = c;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenTripleReason("column", Board::Unit::columnName(which), hidden);
        return true;
    }

    found = !Board::ForEach::box([&](int b, std::vector<int> const & box) {
        if (triple(box, indexes, values, hidden))
        {
            which = b;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenTripleReason("box", Board::Unit::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Hidden::triple(std::vector<int> const & indexes,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       hiddenValues)
{
    // Go through each possible triple of candidates_ and search for exactly three cells containing the one or more of a triple
    for (int x0 = 1; x0 <= Board::SIZE - 2; ++x0)
    {
        Candidates::Type m0 = Candidates::fromValue(x0);
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 1; ++x1)
        {
            Candidates::Type m1 = Candidates::fromValue(x1);
            for (int x2 = x1 + 1; x2 <= Board::SIZE; ++x2)
            {
                Candidates::Type m2 = Candidates::fromValue(x2);
                Candidates::Type m  = m0 | m1 | m2;

                // Count the number of cells with any of these three candidates_
                int nFound    = 0;
                int found[3];
                int counts[3] = { 0 };
                for (int i : indexes)
                {
                    Candidates::Type c = candidates_[i];
                    if (c & m)
                    {
                        // Count the number of found indexes
                        ++nFound;
                        if (nFound > 3)
                            break; // Give up, must be exactly 3
                        found[nFound - 1] = i;

                        // Count the number of times the candidates_ occur
                        if (c & m0)
                            ++counts[0];
                        if (c & m1)
                            ++counts[1];
                        if (c & m2)
                            ++counts[2];
                    }
                }
                // If there are exactly 3 indexes, each candidate is seen at least twice, and there are other candidates_ to
                // eliminate,
                // then success
                if (nFound == 3 && counts[0] >= 2 && counts[1] >= 2 && counts[2] >= 2)
                {
                    Candidates::Type eliminatedCandidates =
                        (candidates_[found[0]] | candidates_[found[1]] | candidates_[found[2]]) &
                        ~m;
                    if (eliminatedCandidates)
                    {
                        eliminatedIndexes.assign(std::begin(found), std::end(found));
                        eliminatedValues = Candidates::values(eliminatedCandidates);
                        hiddenValues     = Candidates::values(m);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static std::string generateHiddenQuadReason(std::string const & unitType, char which, std::vector<int> const & hiddenValues)
{
    std::string reason = "Only these four squares in " + unitType + " " + which +
                         " can be " + std::to_string(hiddenValues[0]) +
                         ", " + std::to_string(hiddenValues[1]) +
                         ", " + std::to_string(hiddenValues[2]) +
                         ", or " + std::to_string(hiddenValues[3]) +
                         ", so they cannot be any other values.";
    return reason;
}

bool Hidden::quadExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive quad in a unit, if they have additional candidates_, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        if (quad(row, indexes, values, hidden))
        {
            which = r;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenQuadReason("row", Board::Unit::rowName(which), hidden);
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        if (quad(column, indexes, values, hidden))
        {
            which = c;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenQuadReason("column", Board::Unit::columnName(which), hidden);
        return true;
    }

    found = !Board::ForEach::box([&](int b, std::vector<int> const & box) {
        if (quad(box, indexes, values, hidden))
        {
            which = b;
            return false; // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        reason = generateHiddenQuadReason("box", Board::Unit::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Hidden::quad(std::vector<int> const & indexes,
                  std::vector<int> &       eliminatedIndexes,
                  std::vector<int> &       eliminatedValues,
                  std::vector<int> &       hiddenValues)
{
    // Go through each possible quad of candidates_ and search for exactly four cells containing the one or more of a quad
    for (int x0 = 1; x0 <= Board::SIZE - 3; ++x0)
    {
        Candidates::Type m0 = Candidates::fromValue(x0);
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 2; ++x1)
        {
            Candidates::Type m1 = Candidates::fromValue(x1);
            for (int x2 = x1 + 1; x2 <= Board::SIZE - 1; ++x2)
            {
                Candidates::Type m2 = Candidates::fromValue(x2);
                for (int x3 = x2 + 1; x3 <= Board::SIZE; ++x3)
                {
                    Candidates::Type m3 = Candidates::fromValue(x3);
                    Candidates::Type m  = m0 | m1 | m2 | m3;

                    // Count the number of cells with any of these four candidates_
                    int nFound    = 0;
                    int found[4];
                    int counts[4] = { 0 };
                    for (int i : indexes)
                    {
                        Candidates::Type c = candidates_[i];
                        if (c & m)
                        {
                            // Count the number of found indexes
                            ++nFound;
                            if (nFound > 4)
                                break; // Give up, must be exactly 4
                            found[nFound - 1] = i;

                            // Count the number of times the candidates_ occur
                            if (c & m0)
                                ++counts[0];
                            if (c & m1)
                                ++counts[1];
                            if (c & m2)
                                ++counts[2];
                            if (c & m3)
                                ++counts[3];
                        }
                    }
                    // If there are exactly 4 indexes, each candidate is seen at least twice, and there are other candidates_ to
                    // eliminate,
                    // then success
                    if (nFound == 4 && counts[0] >= 2 && counts[1] >= 2 && counts[2] >= 2 && counts[3] >= 2)
                    {
                        Candidates::Type eliminatedCandidates =
                            (candidates_[found[0]] | candidates_[found[1]] | candidates_[found[2]] | candidates_[found[3]]) & ~m;
                        if (eliminatedCandidates)
                        {
                            eliminatedIndexes.assign(std::begin(found), std::end(found));
                            eliminatedValues = Candidates::values(eliminatedCandidates);
                            hiddenValues     = Candidates::values(m);
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
