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
static int valueFromMask(unsigned candidates)
{
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
        {
            return v;
        }
    }
    return 0;
}

static std::vector<int> valuesFromMask(unsigned candidates)
{
    std::vector<int> values;
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
        {
            values.push_back(v);
        }
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
        {
            ++count;
        }
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
        return { Step::Type::DONE };
    }

    int r;
    int c;
    int x;

    if (nakedSingle(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE,
                 { Board::indexOf(r, c) },
                 { x },
                 Step::Reason::NAKED_SINGLE,
                 "all other numbers are taken (naked single)" };
    }

    if (hiddenSingleRow(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE,
                 { Board::indexOf(r, c) },
                 { x },
                 Step::Reason::HIDDEN_SINGLE_ROW,
                 "only square in this row that can be this value (hidden single)" };
    }

    if (hiddenSingleColumn(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE,
                 { Board::indexOf(r, c) },
                 { x },
                 Step::Reason::HIDDEN_SINGLE_COLUMN,
                 "only square in this column that can be this value (hidden single)" };
    }

    if (hiddenSingleBox(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE,
                 { Board::indexOf(r, c) },
                 { x },
                 Step::Reason::HIDDEN_SINGLE_BOX,
                 "only square in this box that can be this value (hidden single)" };
    }

    std::vector<int> eliminatedIndexes;
    std::vector<int> eliminatedValues;

    if (nakedPairRow(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_ROW,
                 "two other squares in the row must have one of these two values, so no others can (naked pair)" };
    }

    if (nakedPairColumn(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_COLUMN,
                 "two other squares in the column must have one of these two values, so no others can (naked pair)" };
    }

    if (nakedPairBox(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_BOX,
                 "two other squares in the box must have one of these two values, so no others can (naked pair)" };
    }

    if (nakedTripleRow(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_ROW,
                 "three other squares in the row must have one of these three values, so no others can (naked triple)" };
    }

    if (nakedTripleColumn(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_COLUMN,
                 "three other squares in the column must have one of these three values, so no others can (naked triple)" };
    }

    if (nakedTripleBox(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_BOX,
                 "three other squares in the box must have one of these three values, so no others can (naked triple)" };
    }

    if (nakedQuadRow(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_ROW,
                 "four other squares in the row must have one of these four values, so no others can (naked quad)" };
    }

    if (nakedQuadColumn(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_COLUMN,
                 "four other squares in the column must have one of these four values, so no others can (naked quad)" };
    }

    if (nakedQuadBox(eliminatedIndexes, eliminatedValues))
    {
        eliminate(eliminatedIndexes, eliminatedValues);
        return { Step::Type::ELIMINATE,
                 eliminatedIndexes,
                 eliminatedValues,
                 Step::Reason::NAKED_PAIR_BOX,
                 "four other squares in the box must have one of these four values, so no others can (naked quad)" };
    }

    done_ = true;
    return { Step::Type::STUCK };
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

bool Analyzer::nakedSingle(int * solvedR, int * solvedC, int * solvedValue)
{
    // For each unsolved square, if it only has one candidate, then success
    for (auto i : unsolved_)
    {
        if (solved(candidates_[i]))
        {
            Board::locationOf(i, solvedR, solvedC);
            *solvedValue = valueFromMask(candidates_[i]);
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingleRow(int * solvedR, int * solvedC, int * solvedValue)
{
    // For each unsolved square, if one of its candidates doesn't overlap with the others in its row, then success.
    for (auto s : unsolved_)
    {
        int r;
        int c;
        Board::locationOf(s, &r, &c);

        std::vector<int> indexes = Board::getRowIndexes(r);
        if (hiddenSingle(indexes, s, solvedR, solvedC, solvedValue))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingleColumn(int * solvedR, int * solvedC, int * solvedValue)
{
    // For each unsolved square, if one of its candidates doesn't overlap with the others in its column, then success.
    for (auto s : unsolved_)
    {
        int r;
        int c;
        Board::locationOf(s, &r, &c);

        std::vector<int> indexes = Board::getColumnIndexes(c);
        if (hiddenSingle(indexes, s, solvedR, solvedC, solvedValue))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingleBox(int * solvedR, int * solvedC, int * solvedValue)
{
    // For each unsolved square, if one of its candidates doesn't overlap with the others in its box, then success.
    for (auto s : unsolved_)
    {
        int r;
        int c;
        Board::locationOf(s, &r, &c);

        int r0 = r - (r % Board::BOX_SIZE);
        int c0 = c - (c % Board::BOX_SIZE);
        std::vector<int> indexes = Board::getBoxIndexes(r0, c0);
        if (hiddenSingle(indexes, s, solvedR, solvedC, solvedValue))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingle(std::vector<int> const & indexes, int s, int * solvedR, int * solvedC, int * & solvedValue)
{
    unsigned others = 0;
    for (auto i : indexes)
    {
        if (i != s)
        {
            others |= candidates_[i];
        }
    }

    unsigned exclusive = candidates_[s] & ~others;
    if (!exclusive)
    {
        return false;
    }

    assert(solved(exclusive));
    Board::locationOf(s, solvedR, solvedC);
    *solvedValue = valueFromMask(exclusive);
    return true;
}

bool Analyzer::nakedPairRow(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a row, if there are other candidates that overlap, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> indexes = Board::getRowIndexes(r);
        if (nakedPair(indexes, eliminatedIndexes, eliminatedValues))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::nakedPairColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a column, if there are other candidates that overlap, then success.
    for (int c = 0; c < Board::SIZE; ++c)
    {
        std::vector<int> indexes = Board::getColumnIndexes(c);
        if (nakedPair(indexes, eliminatedIndexes, eliminatedValues))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::nakedPairBox(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a column, if there are other candidates that overlap, then success.
    for (int r0 = 0; r0 < Board::SIZE; r0 += Board::BOX_SIZE)
    {
        for (int c0 = 0; c0 < Board::SIZE; c0 += Board::BOX_SIZE)
        {
            std::vector<int> indexes = Board::getBoxIndexes(r0, c0);
            if (nakedPair(indexes, eliminatedIndexes, eliminatedValues))
            {
                return true;
            }
        }
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
                                {
                                    eliminatedIndexes.push_back(i);
                                }
                            }
                        }
                        if (!eliminatedIndexes.empty())
                        {
                            eliminatedValues = valuesFromMask(candidates);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Analyzer::nakedTripleRow(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a row, if there are other candidates that overlap, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> indexes = Board::getRowIndexes(r);
        if (nakedTriple(indexes, eliminatedIndexes, eliminatedValues))
            return true;
    }
    return false;
}

bool Analyzer::nakedTripleColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a column, if there are other candidates that overlap, then success.
    for (int c = 0; c < Board::SIZE; ++c)
    {
        std::vector<int> indexes = Board::getColumnIndexes(c);
        if (nakedTriple(indexes, eliminatedIndexes, eliminatedValues))
            return true;
    }
    return false;
}

bool Analyzer::nakedTripleBox(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a column, if there are other candidates that overlap, then success.
    for (int r0 = 0; r0 < Board::SIZE; r0 += Board::BOX_SIZE)
    {
        for (int c0 = 0; c0 < Board::SIZE; c0 += Board::BOX_SIZE)
        {
            std::vector<int> indexes = Board::getBoxIndexes(r0, c0);
            if (nakedTriple(indexes, eliminatedIndexes, eliminatedValues))
                return true;
        }
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
                                        {
                                            eliminatedIndexes.push_back(i);
                                        }
                                    }
                                }
                                if (!eliminatedIndexes.empty())
                                {
                                    eliminatedValues = valuesFromMask(candidates);
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

bool Analyzer::nakedQuadRow(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a row, if there are other candidates that overlap, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> indexes = Board::getRowIndexes(r);
        if (nakedQuad(indexes, eliminatedIndexes, eliminatedValues))
            return true;
    }
    return false;
}

bool Analyzer::nakedQuadColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a column, if there are other candidates that overlap, then success.
    for (int c = 0; c < Board::SIZE; ++c)
    {
        std::vector<int> indexes = Board::getColumnIndexes(c);
        if (nakedQuad(indexes, eliminatedIndexes, eliminatedValues))
            return true;
    }
    return false;
}

bool Analyzer::nakedQuadBox(std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
{
    // For each conjugate pair in a column, if there are other candidates that overlap, then success.
    for (int r0 = 0; r0 < Board::SIZE; r0 += Board::BOX_SIZE)
    {
        for (int c0 = 0; c0 < Board::SIZE; c0 += Board::BOX_SIZE)
        {
            std::vector<int> indexes = Board::getBoxIndexes(r0, c0);
            if (nakedQuad(indexes, eliminatedIndexes, eliminatedValues))
                return true;
        }
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
                                                {
                                                    eliminatedIndexes.push_back(i);
                                                }
                                            }
                                        }
                                        if (!eliminatedIndexes.empty())
                                        {
                                            eliminatedValues = valuesFromMask(candidates);
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
