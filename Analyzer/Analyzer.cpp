#include "Analyzer.h"

#include "Board/Board.h"

#include <algorithm>
#include <numeric>

static bool solved(int possible)
{
    return (possible & (possible - 1)) == 0;
}

static int valueFromMask(int unused)
{
    int x = 0;
    while ((unused & 1) == 0)
    {
        unused >>= 1;
        ++x;
    }

    return x;
}

Analyzer::Analyzer(Board const & board)
{
    // Nothing is solved initially
    unsolved_.resize(Board::SIZE * Board::SIZE);
    std::iota(unsolved_.begin(), unsolved_.end(), 0);
    
    // Anything is possible initially
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            int index = r * Board::SIZE + c;
            possible_[index] = 0x3fe;
        }
    }

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

Analyzer::Operation Analyzer::nextOperation()
{
    int r;
    int c;
    int x;
    if (findObviousStep(&r, &c, &x))
    {
        return { Operation::Type::SOLVE, { r * Board::SIZE + c } , x, "All other numbers are taken" };
    }

    return { Operation::Type::STUCK };
}

void Analyzer::solve(int r, int c, int x)
{
    int index = r * Board::SIZE + c;
    possible_[index] = 1 << x;
    unsolved_.erase(std::remove(unsolved_.begin(), unsolved_.end(), index), unsolved_.end());
}

void Analyzer::eliminate(std::vector<int> const & indexes, int x)
{
    for (auto i : indexes)
    {
        possible_[i] &= ~(1 << x);
    }
}

bool Analyzer::findObviousStep(int * r, int * c, int * x)
{
    std::random_shuffle(unsolved_.begin(), unsolved_.end());
    for (auto i : unsolved_)
    {
        int tryR = i / Board::SIZE;
        int tryC =  i % Board::SIZE;

        int unused = 0x3fe;
        std::vector<int> dependents = Board::getDependents(tryR, tryC);
        for (auto d : dependents)
        {
            if (solved(possible_[d]))
            {
                unused &= ~possible_[d];
            }
        }
        if (solved(unused))
        {
            *r = tryR;
            *c = tryC;
            *x = valueFromMask(unused);
            return true;
        }
    }
    return false;
}
