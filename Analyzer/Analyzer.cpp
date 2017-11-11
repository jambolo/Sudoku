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
        return { Step::DONE };
    }

    std::vector<int> indexes;
    std::vector<int> values;

    if (nakedSingle(indexes, values))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE,
                 indexes,
                 values,
                 Step::NAKED_SINGLE,
                 "there are no other possibilities for this square (naked single)" };
    }

    if (hiddenSingleRow(indexes, values))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE,
                 indexes,
                 values,
                 Step::HIDDEN_SINGLE,
                 "only square in this row that can be this value (hidden single)" };
    }

    if (hiddenSingleColumn(indexes, values))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE,
                 indexes,
                 values,
                 Step::HIDDEN_SINGLE,
                 "only square in this column that can be this value (hidden single)" };
    }

    if (hiddenSingleBox(indexes, values))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE,
                 indexes,
                 values,
                 Step::HIDDEN_SINGLE,
                 "only square in this box that can be this value (hidden single)" };
    }

    if (nakedPairRow(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_PAIR,
                 "two other squares in the row must be one of these two values, so no others can (naked pair)" };
    }

    if (nakedPairColumn(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_PAIR,
                 "two other squares in the column must be one of these two values, so no others can (naked pair)" };
    }

    if (nakedPairBox(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_PAIR,
                 "two other squares in the box must be one of these two values, so no others can (naked pair)" };
    }

    if (hiddenPairRow(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_PAIR,
                 "only these two squares in the row can be one of two values, so they cannot be any other value (hidden pair)" };
    }

    if (hiddenPairColumn(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_PAIR,
                 "only these two squares in the column can be one of two values, so they cannot be any other value (hidden pair)" };
    }

    if (hiddenPairBox(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_PAIR,
                 "only these two squares in the box can be one of two values, so they cannot be any other value (hidden pair)" };
    }

    if (nakedTripleRow(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_TRIPLE,
                 "three other squares in the row must be one of these three values, so no others can (naked triple)" };
    }

    if (nakedTripleColumn(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_TRIPLE,
                 "three other squares in the column must be one of these three values, so no others can (naked triple)" };
    }

    if (nakedTripleBox(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_TRIPLE,
                 "three other squares in the box must be one of these three values, so no others can (naked triple)" };
    }

    if (hiddenTripleRow(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_TRIPLE,
                 "only these three squares in the box can be one of three values, so they cannot be any other value (hidden triple)" };
    }

    if (hiddenTripleColumn(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_TRIPLE,
                 "only these three squares in the box can be one of three values, so they cannot be any other value (hidden triple)" };
    }

    if (hiddenTripleBox(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_TRIPLE,
                 "only these three squares in the box can be one of three values, so they cannot be any other value (hidden triple)" };
    }

    if (nakedQuadRow(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_QUAD,
                 "four other squares in the row must be one of these four values, so no others can (naked quad)" };
    }

    if (nakedQuadColumn(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_QUAD,
                 "four other squares in the column must be one of these four values, so no others can (naked quad)" };
    }

    if (nakedQuadBox(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::NAKED_QUAD,
                 "four other squares in the box must be one of these four values, so no others can (naked quad)" };
    }

    if (hiddenQuadRow(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_QUAD,
                 "only these four squares in the row can be one of four values, so they cannot be any other value (hidden quad)" };
    }

    if (hiddenQuadColumn(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_QUAD,
                 "only these four squares in the box can be one of four values, so they cannot be any other value (hidden quad)" };
    }

    if (hiddenQuadBox(indexes, values))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE,
                 indexes,
                 values,
                 Step::HIDDEN_QUAD,
                 "only these four squares in the box can be one of four values, so they cannot be any other value (hidden quad)" };
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

bool Analyzer::hiddenSingleRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each unsolved square, if one of its candidates doesn't overlap with the others in its row, then success.
    for (auto s : unsolved_)
    {
        int r;
        int c;
        Board::locationOf(s, &r, &c);

        std::vector<int> row = Board::getRowIndexes(r);
        if (hiddenSingle(row, s, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingleColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each unsolved square, if one of its candidates doesn't overlap with the others in its column, then success.
    for (auto s : unsolved_)
    {
        int r;
        int c;
        Board::locationOf(s, &r, &c);

        std::vector<int> column = Board::getColumnIndexes(c);
        if (hiddenSingle(column, s, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingleBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each unsolved square, if one of its candidates doesn't overlap with the others in its box, then success.
    for (auto s : unsolved_)
    {
        int r;
        int c;
        Board::locationOf(s, &r, &c);

        int r0 = r - (r % Board::BOX_SIZE);
        int c0 = c - (c % Board::BOX_SIZE);
        std::vector<int> box = Board::getBoxIndexes(r0, c0);
        if (hiddenSingle(box, s, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenSingle(std::vector<int> const & indexes, int s, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues)
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
    eliminatedIndexes.push_back(s);
    eliminatedValues.push_back(valueFromMask(exclusive));
    return true;
}

bool Analyzer::hiddenPairRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive pair in a row, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> row = Board::getRowIndexes(r);
        if (hiddenPair(row, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenPairColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive pair in a column, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> column = Board::getRowIndexes(r);
        if (hiddenPair(column, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenPairBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive pair in a box, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> box = Board::getRowIndexes(r);
        if (hiddenPair(box, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenPair(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues)
{
    return false;
}

bool Analyzer::hiddenTripleRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive triple in a row, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> row = Board::getRowIndexes(r);
        if (hiddenTriple(row, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenTripleColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive triple in a column, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> column = Board::getRowIndexes(r);
        if (hiddenTriple(column, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenTripleBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive triple in a box, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> box = Board::getRowIndexes(r);
        if (hiddenTriple(box, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenTriple(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues)
{
    return false;
}

bool Analyzer::hiddenQuadRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive quad in a row, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> row = Board::getRowIndexes(r);
        if (hiddenQuad(row, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenQuadColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive quad in a column, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> column = Board::getRowIndexes(r);
        if (hiddenQuad(column, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenQuadBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive quad in a box, if they have additional candidates, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> box = Board::getRowIndexes(r);
        if (hiddenQuad(box, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::hiddenQuad(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues)
{
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
            values.push_back(valueFromMask(candidates));
            return true;
        }
    }
    return false;
}

bool Analyzer::nakedPairRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive pair in a row, if there are other candidates that overlap, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> row = Board::getRowIndexes(r);
        if (nakedPair(row, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::nakedPairColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive pair in a column, if there are other candidates that overlap, then success.
    for (int c = 0; c < Board::SIZE; ++c)
    {
        std::vector<int> column = Board::getColumnIndexes(c);
        if (nakedPair(column, indexes, values))
        {
            return true;
        }
    }
    return false;
}

bool Analyzer::nakedPairBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive pair in a column, if there are other candidates that overlap, then success.
    for (int r0 = 0; r0 < Board::SIZE; r0 += Board::BOX_SIZE)
    {
        for (int c0 = 0; c0 < Board::SIZE; c0 += Board::BOX_SIZE)
        {
            std::vector<int> box = Board::getBoxIndexes(r0, c0);
            if (nakedPair(box, indexes, values))
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

bool Analyzer::nakedTripleRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive triple in a row, if there are other candidates that overlap, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> row = Board::getRowIndexes(r);
        if (nakedTriple(row, indexes, values))
            return true;
    }
    return false;
}

bool Analyzer::nakedTripleColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive triple in a column, if there are other candidates that overlap, then success.
    for (int c = 0; c < Board::SIZE; ++c)
    {
        std::vector<int> column = Board::getColumnIndexes(c);
        if (nakedTriple(column, indexes, values))
            return true;
    }
    return false;
}

bool Analyzer::nakedTripleBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive triple in a column, if there are other candidates that overlap, then success.
    for (int r0 = 0; r0 < Board::SIZE; r0 += Board::BOX_SIZE)
    {
        for (int c0 = 0; c0 < Board::SIZE; c0 += Board::BOX_SIZE)
        {
            std::vector<int> box = Board::getBoxIndexes(r0, c0);
            if (nakedTriple(box, indexes, values))
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

bool Analyzer::nakedQuadRow(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive quad in a row, if there are other candidates that overlap, then success.
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::vector<int> row = Board::getRowIndexes(r);
        if (nakedQuad(row, indexes, values))
            return true;
    }
    return false;
}

bool Analyzer::nakedQuadColumn(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive quad in a column, if there are other candidates that overlap, then success.
    for (int c = 0; c < Board::SIZE; ++c)
    {
        std::vector<int> column = Board::getColumnIndexes(c);
        if (nakedQuad(column, indexes, values))
            return true;
    }
    return false;
}

bool Analyzer::nakedQuadBox(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each exclusive quad in a column, if there are other candidates that overlap, then success.
    for (int r0 = 0; r0 < Board::SIZE; r0 += Board::BOX_SIZE)
    {
        for (int c0 = 0; c0 < Board::SIZE; c0 += Board::BOX_SIZE)
        {
            std::vector<int> box = Board::getBoxIndexes(r0, c0);
            if (nakedQuad(box, indexes, values))
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
