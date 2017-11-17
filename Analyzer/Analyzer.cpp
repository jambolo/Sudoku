#include "Analyzer.h"

#include "Board/Board.h"

#include <algorithm>
#include <cassert>
#include <numeric>

static int const ALL_CANDIDATES = 0x3fe;

// Returns true if there is only one candidate
static bool solved(unsigned candidates)
{
    return (candidates & (candidates - 1)) == 0;
}

// Returns the value of the only candidate
static int valueFromCandidates(unsigned candidates)
{
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
            return v;
    }
    return 0;
}

// Returns a list of of candidates
static std::vector<int> valuesFromCandidates(unsigned candidates)
{
    std::vector<int> values;
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
            values.push_back(v);
    }
    return values;
}

static int candidateCount(unsigned candidates)
{
    int count = 0;
    while (candidates != 0)
    {
        candidates >>= 1;
        if (candidates & 1)
            ++count;
    }
    return count;
}

Analyzer::Analyzer(Board const & board)
    : board_(board)
    , unsolved_(Board::SIZE * Board::SIZE)
    , candidates_(Board::SIZE * Board::SIZE, ALL_CANDIDATES)
    , done_(false)
{
    // Nothing is solved initially
    std::iota(unsolved_.begin(), unsolved_.end(), 0);

    // For solved squares, mark as solved
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            if (!board.isEmpty(r, c))
            {
                int x = board.get(r, c);
                solve(r, c, x);
            }
        }
    }
}

Analyzer::Step Analyzer::next()
{
    if (board_.completed())
    {
        done_ = true;
        return { Step::DONE };
    }

    std::vector<int> indexes;
    std::vector<int> values;
    std::string reason;

    if (nakedSingleFound(indexes, values, reason))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE, indexes, values, Step::NAKED_SINGLE, reason };
    }

    if (hiddenSingleFound(indexes, values, reason))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE, indexes, values, Step::HIDDEN_SINGLE, reason };
    }

    if (nakedPairFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::NAKED_PAIR, reason };
    }

    if (nakedTripleFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::NAKED_TRIPLE, reason };
    }

    if (nakedQuadFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::NAKED_QUAD, reason };
    }

    if (lockedCandidatesFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::LOCKED_CANDIDATES, reason };
    }

    if (hiddenPairFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_PAIR, reason };
    }

    if (hiddenTripleFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_TRIPLE, reason };
    }

    if (hiddenQuadFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_QUAD, reason };
    }

    done_ = true;
    return { Step::STUCK };
}

void Analyzer::solve(int r, int c, int x)
{
    // Update the board
    board_.set(r, c, x);

    // The square has only one candidate now
    int index = Board::indexOf(r, c);
    candidates_[index] = 1 << x;

    // Remove from the list of unsolved squares
    unsolved_.erase(std::remove(unsolved_.begin(), unsolved_.end(), index), unsolved_.end());

    // Eliminate this square's value from its dependents' candidates
    std::vector<int> dependents = Board::getDependents(r, c);
    eliminate(dependents, x);
}

void Analyzer::eliminate(std::vector<int> const & indexes, int x)
{
    for (auto i : indexes)
    {
        candidates_[i] &= ~(1 << x);
        assert(candidates_[i] != 0);
    }
}

void Analyzer::eliminate(std::vector<int> const & indexes, std::vector<int> const & values)
{
    for (auto v : values)
    {
        eliminate(indexes, v);
    }
}

bool Analyzer::hiddenSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    bool found;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !hiddenSingle(row, indexes, values);
    });
    if (found)
    {
        reason = "this is the only square in this row that can be this value";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !hiddenSingle(column, indexes, values);
    });
    if (found)
    {
        reason = "this is the only square in this column that can be this value";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !hiddenSingle(box, indexes, values);
    });
    if (found)
    {
        reason = "this is the only square in this box that can be this value";
        return true;
    }

    return false;
}

bool Analyzer::hiddenSingle(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues)
{
    for (int s : indexes)
    {
        if (board_.isEmpty(s))
        {
            unsigned others = 0;
            for (int i : indexes)
            {
                if (i != s)
                    others |= candidates_[i];
            }

            unsigned exclusive = candidates_[s] & ~others;
            if (exclusive)
            {
                assert(solved(exclusive));
                eliminatedIndexes.push_back(s);
                eliminatedValues.push_back(valueFromCandidates(exclusive));
                return true;
            }
        }
    }
    return false;
}

bool Analyzer::hiddenPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive pair in a unit, if they have additional candidates, then success.

    bool found;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !hiddenPair(row, indexes, values);
    });
    if (found)
    {
        reason = "only these two squares in the row can be one of two values, so they cannot be any other values";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !hiddenPair(column, indexes, values);
    });
    if (found)
    {
        reason = "only these two squares in the column can be one of two values, so they cannot be any other values";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !hiddenPair(box, indexes, values);
    });
    if (found)
    {
        reason = "only these two squares in the box can be one of two values, so they cannot be any other values";
        return true;
    }

    return false;
}

bool Analyzer::hiddenPair(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues)
{
    // Go through each possible pair of candidates and search for exactly two cells containing the one or more of a pair
    for (int x0 = 1; x0 <= Board::SIZE - 1; ++x0)
    {
        unsigned m0 = 1 << x0;
        for (int x1 = x0 + 1; x1 <= Board::SIZE; ++x1)
        {
            unsigned m1 = 1 << x1;
            unsigned m  = m0 | m1;

            // Count the number of cells with either of these two candidates
            int nFound    = 0;
            int found[2];
            int counts[2] = { 0 };
            for (int i : indexes)
            {
                unsigned candidates = candidates_[i];
                if (candidates & m)
                {
                    // Count the number of found indexes
                    ++nFound;
                    if (nFound > 2)
                        break;  // Give up, must be exactly 2
                    found[nFound - 1] = i;

                    // Count the number of times the candidates occur
                    if (candidates & m0)
                        ++counts[0];
                    if (candidates & m1)
                        ++counts[1];
                }
            }
            // If there are exactly 2 indexes, each candidate is seen at least twice, and there are other candidates to eliminate,
            // then success
            if (nFound == 2 && counts[0] >= 2 && counts[1] >= 2)
            {
                unsigned eliminatedCandidates = (candidates_[found[0]] | candidates_[found[1]]) & ~m;
                if (eliminatedCandidates)
                {
                    eliminatedIndexes.assign(std::begin(found), std::end(found));
                    eliminatedValues = valuesFromCandidates(eliminatedCandidates);
                    return true;
                }
            }
        }
    }

    return false;
}

bool Analyzer::hiddenTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive triple in a unit, if they have additional candidates, then success.

    bool found;
    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !hiddenTriple(row, indexes, values);
    });
    if (found)
    {
        reason = "only these three squares in the row can be one of three values, so they cannot be any other values";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !hiddenTriple(column, indexes, values);
    });
    if (found)
    {
        reason = "only these three squares in the column can be one of three values, so they cannot be any other values";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !hiddenTriple(box, indexes, values);
    });
    if (found)
    {
        reason = "only these three squares in the box can be one of three values, so they cannot be any other values";
        return true;
    }

    return false;
}

bool Analyzer::hiddenTriple(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues)
{
    // Go through each possible triple of candidates and search for exactly three cells containing the one or more of a triple
    for (int x0 = 1; x0 <= Board::SIZE - 2; ++x0)
    {
        unsigned m0 = 1 << x0;
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 1; ++x1)
        {
            unsigned m1 = 1 << x1;
            for (int x2 = x1 + 1; x2 <= Board::SIZE; ++x2)
            {
                unsigned m2 = 1 << x2;
                unsigned m  = m0 | m1 | m2;

                // Count the number of cells with any of these three candidates
                int nFound    = 0;
                int found[3];
                int counts[3] = { 0 };
                for (int i : indexes)
                {
                    unsigned candidates = candidates_[i];
                    if (candidates & m)
                    {
                        // Count the number of found indexes
                        ++nFound;
                        if (nFound > 3)
                            break;  // Give up, must be exactly 3
                        found[nFound - 1] = i;

                        // Count the number of times the candidates occur
                        if (candidates & m0)
                            ++counts[0];
                        if (candidates & m1)
                            ++counts[1];
                        if (candidates & m2)
                            ++counts[2];
                    }
                }
                // If there are exactly 3 indexes, each candidate is seen at least twice, and there are other candidates to
                // eliminate,
                // then success
                if (nFound == 3 && counts[0] >= 2 && counts[1] >= 2 && counts[2] >= 2)
                {
                    unsigned eliminatedCandidates = (candidates_[found[0]] | candidates_[found[1]] | candidates_[found[2]]) & ~m;
                    if (eliminatedCandidates)
                    {
                        eliminatedIndexes.assign(std::begin(found), std::end(found));
                        eliminatedValues = valuesFromCandidates(eliminatedCandidates);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Analyzer::hiddenQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    bool found;
    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !hiddenQuad(row, indexes, values);
    });
    if (found)
    {
        reason = "only these four squares in the row can be one of four values, so they cannot be any other values";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !hiddenQuad(column, indexes, values);
    });
    if (found)
    {
        reason = "only these four squares in the column can be one of four values, so they cannot be any other values";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !hiddenQuad(box, indexes, values);
    });
    if (found)
    {
        reason = "only these four squares in the box can be one of four values, so they cannot be any other values";
        return true;
    }

    return false;
}

bool Analyzer::hiddenQuad(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues)
{
    // Go through each possible quad of candidates and search for exactly four cells containing the one or more of a quad
    for (int x0 = 1; x0 <= Board::SIZE - 3; ++x0)
    {
        unsigned m0 = 1 << x0;
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 2; ++x1)
        {
            unsigned m1 = 1 << x1;
            for (int x2 = x1 + 1; x2 <= Board::SIZE - 1; ++x2)
            {
                unsigned m2 = 1 << x2;
                for (int x3 = x2 + 1; x3 <= Board::SIZE; ++x3)
                {
                    unsigned m3 = 1 << x3;
                    unsigned m  = m0 | m1 | m2 | m3;

                    // Count the number of cells with any of these four candidates
                    int nFound    = 0;
                    int found[4];
                    int counts[4] = { 0 };
                    for (int i : indexes)
                    {
                        unsigned candidates = candidates_[i];
                        if (candidates & m)
                        {
                            // Count the number of found indexes
                            ++nFound;
                            if (nFound > 4)
                                break;  // Give up, must be exactly 4
                            found[nFound - 1] = i;

                            // Count the number of times the candidates occur
                            if (candidates & m0)
                                ++counts[0];
                            if (candidates & m1)
                                ++counts[1];
                            if (candidates & m2)
                                ++counts[2];
                            if (candidates & m3)
                                ++counts[3];
                        }
                    }
                    // If there are exactly 4 indexes, each candidate is seen at least twice, and there are other candidates to
                    // eliminate,
                    // then success
                    if (nFound == 4 && counts[0] >= 2 && counts[1] >= 2 && counts[2] >= 2 && counts[3] >= 2)
                    {
                        unsigned eliminatedCandidates =
                            (candidates_[found[0]] | candidates_[found[1]] | candidates_[found[2]] | candidates_[found[3]]) & ~m;
                        if (eliminatedCandidates)
                        {
                            eliminatedIndexes.assign(std::begin(found), std::end(found));
                            eliminatedValues = valuesFromCandidates(eliminatedCandidates);
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Analyzer::nakedSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    if (nakedSingle(indexes, values))
    {
        reason = "there are no other possible values for this square";
        return true;
    }
    return false;
}

bool Analyzer::nakedSingle(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each unsolved square, if it only has one candidate, then success
    for (auto i : unsolved_)
    {
        unsigned candidates = candidates_[i];
        if (solved(candidates))
        {
            indexes.push_back(i);
            values.push_back(valueFromCandidates(candidates));
            return true;
        }
    }
    return false;
}

bool Analyzer::nakedPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    bool found;
    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !nakedPair(row, indexes, values);
    });
    if (found)
    {
        reason = "two other squares in the row must be one of these two values, so no others can";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !nakedPair(column, indexes, values);
    });
    if (found)
    {
        reason = "two other squares in the column must be one of these two values, so no others can";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !nakedPair(box, indexes, values);
    });
    if (found)
    {
        reason = "two other squares in the box must be one of these two values, so no others can";
        return true;
    }

    return false;
}

bool Analyzer::nakedPair(std::vector<int> const & indexes,
                         std::vector<int> &       eliminatedIndexes,
                         std::vector<int> &       eliminatedValues)
{
    for (int b0 = 0; b0 < Board::SIZE - 1; ++b0)
    {
        int i0     = indexes[b0];
        int count0 = candidateCount(candidates_[i0]);
        if (count0 > 1 && count0 <= 2)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE; ++b1)
            {
                int i1     = indexes[b1];
                int count1 = candidateCount(candidates_[i1]);
                if (count1 > 1 && count1 <= 2)
                {
                    unsigned candidates = candidates_[i0] | candidates_[i1];
                    if (candidateCount(candidates) == 2)
                    {
                        for (auto i : indexes)
                        {
                            if (i != i0 && i != i1)
                            {
                                unsigned eliminated = candidates_[i] & candidates;
                                if (eliminated)
                                    eliminatedIndexes.push_back(i);
                            }
                        }
                        if (!eliminatedIndexes.empty())
                        {
                            eliminatedValues = valuesFromCandidates(candidates);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Analyzer::nakedTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive triple in a unit, if there are other candidates that overlap, then success.
    bool found;
    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !nakedTriple(row, indexes, values);
    });
    if (found)
    {
        reason = "three other squares in the row must be one of these three values, so no others can";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !nakedTriple(column, indexes, values);
    });
    if (found)
    {
        reason = "three other squares in the column must be one of these three values, so no others can";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !nakedTriple(box, indexes, values);
    });
    if (found)
    {
        reason = "three other squares in the box must be one of these three values, so no others can";
        return true;
    }

    return false;
}

bool Analyzer::nakedTriple(std::vector<int> const & indexes,
                           std::vector<int> &       eliminatedIndexes,
                           std::vector<int> &       eliminatedValues)
{
    for (int b0 = 0; b0 < Board::SIZE - 2; ++b0)
    {
        int i0     = indexes[b0];
        int count0 = candidateCount(candidates_[i0]);
        if (count0 > 1 && count0 <= 3)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 1; ++b1)
            {
                int i1     = indexes[b1];
                int count1 = candidateCount(candidates_[i1]);
                if (count1 > 1 && count1 <= 3)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE; ++b2)
                    {
                        int i2     = indexes[b2];
                        int count2 = candidateCount(candidates_[i2]);
                        if (count2 > 1 && count2 <= 3)
                        {
                            unsigned candidates = candidates_[i0] | candidates_[i1] | candidates_[i2];
                            if (candidateCount(candidates) == 3)
                            {
                                for (auto i : indexes)
                                {
                                    if (i != i0 && i != i1 && i != i2)
                                    {
                                        unsigned eliminated = candidates_[i] & candidates;
                                        if (eliminated)
                                            eliminatedIndexes.push_back(i);
                                    }
                                }
                                if (!eliminatedIndexes.empty())
                                {
                                    eliminatedValues = valuesFromCandidates(candidates);
                                    return true;
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

bool Analyzer::nakedQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive quad in a unit, if there are other candidates that overlap, then success.
    bool found;
    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
        return !nakedQuad(row, indexes, values);
    });
    if (found)
    {
        reason = "four other squares in the row must be one of these four values, so no others can";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
        return !nakedQuad(column, indexes, values);
    });
    if (found)
    {
        reason = "four other squares in the column must be one of these four values, so no others can";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
        return !nakedQuad(box, indexes, values);
    });
    if (found)
    {
        reason = "four other squares in the box must be one of these four values, so no others can";
        return true;
    }

    return false;
}

bool Analyzer::nakedQuad(std::vector<int> const & indexes,
                         std::vector<int> &       eliminatedIndexes,
                         std::vector<int> &       eliminatedValues)
{
    for (int b0 = 0; b0 < Board::SIZE - 3; ++b0)
    {
        int i0     = indexes[b0];
        int count0 = candidateCount(candidates_[i0]);
        if (count0 > 1 && count0 <= 4)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 2; ++b1)
            {
                int i1     = indexes[b1];
                int count1 = candidateCount(candidates_[i1]);
                if (count1 > 1 && count1 <= 4)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE - 1; ++b2)
                    {
                        int i2     = indexes[b2];
                        int count2 = candidateCount(candidates_[i2]);
                        if (count2 > 1 && count2 <= 4)
                        {
                            for (int b3 = b2 + 1; b3 < Board::SIZE; ++b3)
                            {
                                int i3     = indexes[b3];
                                int count3 = candidateCount(candidates_[i3]);
                                if (count3 > 1 && count3 <= 4)
                                {
                                    unsigned candidates = candidates_[i0] | candidates_[i1] | candidates_[i2] | candidates_[i3];
                                    if (candidateCount(candidates) == 4)
                                    {
                                        for (auto i : indexes)
                                        {
                                            if (i != i0 && i != i1 && i != i2 && i != i3)
                                            {
                                                unsigned eliminated = candidates_[i] & candidates;
                                                if (eliminated)
                                                    eliminatedIndexes.push_back(i);
                                            }
                                        }
                                        if (!eliminatedIndexes.empty())
                                        {
                                            eliminatedValues = valuesFromCandidates(candidates);
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
    }
    return false;
}

bool Analyzer::lockedCandidatesFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the row/column, then success if those candidates exist in the box.
    bool found;

    found = !board_.for_each_row([&](int r, std::vector<int> const & row) {
        for (int i = 0; i < Board::SIZE / Board::BOX_SIZE; ++i)
        {
            int r, c;
            Board::locationOf(row[i * Board::BOX_SIZE], &r, &c);
            std::vector<int> box = Board::getBoxIndexes(Board::indexOfBox(r, c));
            if (lockedCandidates(row, box, indexes, values))
                return false;
        }
        return true;
    });
    if (found)
    {
        reason = "since the part of the box within the row must contain these values, they cannot be anywhere else in the box";
        return true;
    }

    found = !board_.for_each_column([&](int c, std::vector<int> const & column) {
        for (int i = 0; i < Board::SIZE / Board::BOX_SIZE; ++i)
        {
            int r, c;
            Board::locationOf(column[i * Board::BOX_SIZE], &r, &c);
            std::vector<int> box = Board::getBoxIndexes(Board::indexOfBox(r, c));
            if (lockedCandidates(column, box, indexes, values))
                return false;
        }
        return true;
    });
    if (found)
    {
        reason = "since the part of the box within the column must contain these values, they cannot be anywhere else in the box";
        return true;
    }

    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the box, then success if those candidates exist in the row/column.

    found = !board_.for_each_row([&](int r, std::vector<int> const & row) {
        for (int i = 0; i < Board::SIZE / Board::BOX_SIZE; ++i)
        {
            int r, c;
            Board::locationOf(row[i * Board::BOX_SIZE], &r, &c);
            std::vector<int> box = Board::getBoxIndexes(Board::indexOfBox(r, c));
            if (lockedCandidates(box, row, indexes, values))
                return false;
        }
        return true;
    });
    if (found)
    {
        reason = "since the part of the row within the box must contain these values, they cannot be anywhere else in the row";
        return true;
    }

    found = !board_.for_each_column([&](int c, std::vector<int> const & column) {
        for (int i = 0; i < Board::SIZE / Board::BOX_SIZE; ++i)
        {
            int r, c;
            Board::locationOf(column[i * Board::BOX_SIZE], &r, &c);
            std::vector<int> box = Board::getBoxIndexes(Board::indexOfBox(r, c));
            if (lockedCandidates(box, column, indexes, values))
                return false;
        }
        return true;
    });
    if (found)
    {
        reason =
            "since the part of the column within the box must contain these values, they cannot be anywhere else in the column";
        return true;
    }

    return false;
}

bool Analyzer::lockedCandidates(std::vector<int> const & indexes1,
                                std::vector<int> const & indexes2,
                                std::vector<int> &       eliminatedIndexes,
                                std::vector<int> &       eliminatedValues)
{
    // Indexes in the intersection
    std::vector<int> intersection(Board::BOX_SIZE);
    std::set_intersection(indexes1.begin(), indexes1.end(), indexes2.begin(), indexes2.end(), intersection.begin());

    // Indexes not in the intersection
    std::vector<int> others1(Board::SIZE - Board::BOX_SIZE);
    std::set_difference(indexes1.begin(), indexes1.end(), intersection.begin(), intersection.end(), others1.begin());
    std::vector<int> others2(Board::SIZE - Board::BOX_SIZE);
    std::set_difference(indexes2.begin(), indexes2.end(), intersection.begin(), intersection.end(), others2.begin());

    // Candidates in the intersection
    unsigned intersectionCandidates = 0;
    for (int i : intersection)
    {
        if (!solved(candidates_[i]))
            intersectionCandidates |= candidates_[i];
    }

    // Candidates in set 1, not in intersection
    unsigned otherCandidates1 = 0;
    for (int i : others1)
    {
        if (!solved(candidates_[i]))
            otherCandidates1 |= candidates_[i];
    }

    // If any of the candidates in the intersection don't exist in the rest of set 1, then eliminate them from set 2
    unsigned unique1 = intersectionCandidates & ~otherCandidates1;
    if (unique1)
    {
        bool found = false;
        for (int i : others2)
        {
            if (candidates_[i] & unique1)
            {
                eliminatedIndexes.push_back(i);
                found = true;
            }
        }
        if (found)
        {
            std::vector<int> uniqueCandidates = valuesFromCandidates(unique1);
            eliminatedValues.insert(eliminatedValues.end(), uniqueCandidates.begin(), uniqueCandidates.end());
        }
    }

    std::sort(eliminatedIndexes.begin(), eliminatedIndexes.end());
    eliminatedIndexes.erase(std::unique(eliminatedIndexes.begin(), eliminatedIndexes.end()), eliminatedIndexes.end());
    std::sort(eliminatedValues.begin(), eliminatedValues.end());
    eliminatedValues.erase(std::unique(eliminatedValues.begin(), eliminatedValues.end()), eliminatedValues.end());

    return !eliminatedIndexes.empty();
}

const char * Analyzer::Step::techniqueName(Analyzer::Step::TechniqueId technique)
{
    static char const * const NAMES[] =
    {
        "none",
        "hidden single",
        "hidden pair",
        "hidden triple",
        "hidden quad",
        "naked single",
        "naked pair",
        "naked triple",
        "naked quad",
        "locked candidates"
    };
    assert((size_t)technique >= 0 && (size_t)technique < sizeof(NAMES) / sizeof(*NAMES));
    return NAMES[technique];
}
