#include "Analyzer.h"

#include "Board/Board.h"

#include <algorithm>
#include <cassert>
#include <numeric>

static int const ALL_POSSIBLE_MASK = 0x3fe;

// Returns true if there is only one possibility
static bool solved(int possible)
{
    return (possible & (possible - 1)) == 0;
}

// Returns the value of the only possibility
static int valueFromMask(int mask)
{
    int x = 0;
    while ((mask & 1) == 0)
    {
        mask >>= 1;
        ++x;
    }

    return x;
}

Analyzer::Analyzer(Board const & board)
    : board_(board)
    , unsolved_(Board::SIZE * Board::SIZE)
    , possibilities_(Board::SIZE * Board::SIZE, ALL_POSSIBLE_MASK)
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

    if (allOtherNumbersTaken(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE, { Board::indexOf(r, c) }, x, "all other numbers are taken" };
    }

    if (onlySquareInRowForThisValue(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE, { Board::indexOf(r, c) }, x, "only square available in this row for this value" };
    }

    if (onlySquareInColumnForThisValue(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE, { Board::indexOf(r, c) }, x, "only square available in this column for this value" };
    }

    if (onlySquareInBoxForThisValue(&r, &c, &x))
    {
        solve(r, c, x);
        return { Step::Type::SOLVE, { Board::indexOf(r, c) }, x, "only square available in this box for this value" };
    }

    done_ = true;
    return { Step::Type::STUCK };
}

void Analyzer::solve(int r, int c, int x)
{
    // Update the board
    board_.set(r, c, x);

    // The square has only one possible value now
    int index = Board::indexOf(r, c);
    possibilities_[index] = 1 << x;

    // Remove from the list of unsolved squares
    unsolved_.erase(std::remove(unsolved_.begin(), unsolved_.end(), index), unsolved_.end());

    // Eliminate this square's value from possibilities for its dependents
    std::vector<int> dependents = Board::getDependents(r, c);
    eliminate(dependents, x);
}

void Analyzer::eliminate(std::vector<int> const & indexes, int x)
{
    for (auto i : indexes)
    {
        possibilities_[i] &= ~(1 << x);
        assert(possibilities_[i] != 0);
    }
}

bool Analyzer::allOtherNumbersTaken(int * r, int * c, int * x)
{
    // For each unsolved square, if it only has one possible value, then success
    for (auto i : unsolved_)
    {
        if (solved(possibilities_[i]))
        {
            Board::locationOf(i, r, c);
            *x = valueFromMask(possibilities_[i]);
            return true;
        }
    }
    return false;
}

bool Analyzer::onlySquareInRowForThisValue(int * r, int * c, int * x)
{
    // For each unsolved square, if one of its possible values doesn't overlap with the others in its row, then success.
    for (auto s : unsolved_)
    {
        int i;
        int j;
        Board::locationOf(s, &i, &j);

        int others = 0;
        for (int k = 0; k < Board::SIZE; ++k)
        {
            if (k != j)
            {
                others |= possibilities_[Board::indexOf(i, k)];
            }
        }
        int exclusive = possibilities_[Board::indexOf(i, j)] & ~others;
        if (exclusive != 0)
        {
            assert(solved(exclusive));
            *r = i;
            *c = j;
            *x = valueFromMask(exclusive);
            return true;
        }
    }
    return false;
}

bool Analyzer::onlySquareInColumnForThisValue(int * r, int * c, int * x)
{
    // For each unsolved square, if one of its possible values doesn't overlap with the others in its column, then success.
    for (auto s : unsolved_)
    {
        int i;
        int j;
        Board::locationOf(s, &i, &j);

        int others = 0;
        for (int k = 0; k < Board::SIZE; ++k)
        {
            if (k != i)
            {
                others |= possibilities_[Board::indexOf(k, j)];
            }
        }
        int exclusive = possibilities_[Board::indexOf(i, j)] & ~others;
        if (exclusive != 0)
        {
            assert(solved(exclusive));
            *r = i;
            *c = j;
            *x = valueFromMask(exclusive);
            return true;
        }
    }
    return false;
}

bool Analyzer::onlySquareInBoxForThisValue(int * r, int * c, int * x)
{
    // For each unsolved square, if one of its possible values doesn't overlap with the others in its box, then success.
    for (auto s : unsolved_)
    {
        int i;
        int j;
        Board::locationOf(s, &i, &j);
        
        int i0 = i - (i % Board::BOX_SIZE);
        int j0 = j - (j % Board::BOX_SIZE);

        int others = 0;
        for (int m = 0; m < Board::BOX_SIZE; ++m)
        {
            for (int n = 0; n < Board::BOX_SIZE; ++n)
            {
                if (i0 + m != i || j0 + n != j)
                {
                    others |= possibilities_[Board::indexOf(i0 + m, j0 + n)];
                }
            }
        }
        int exclusive = possibilities_[Board::indexOf(i, j)] & ~others;
        if (exclusive != 0)
        {
            assert(solved(exclusive));
            *r = i;
            *c = j;
            *x = valueFromMask(exclusive);
            return true;
        }
    }
    return false;
}
