#include "Generator.h"

#include "Board/Board.h"
#include "Solver/Solver.h"

#include <algorithm>
#include <numeric>
#include <vector>

Board Generator::generate(int difficulty/* = 0*/)
{
    if (difficulty <= 0)
        difficulty = Board::SIZE * Board::SIZE;

    // Generate a random solved board
    Board b;
    attempt(b, 0, -1);

    // Randomly remove as many squares as possible until no unique solution can be found
    std::vector<int> squares = randomizedSquares();
    for (auto p : squares)
    {
        int r = p / Board::SIZE;
        int c = p % Board::SIZE;
        int x = b.get(r, c);
        b.set(r, c, Board::EMPTY);
        if (Solver::hasUniqueSolution(b))
        {
            if (--difficulty <= 0)
                return b;
        }
        else
        {
            b.set(r, c, x); // Skip this one
        }
    }
    return b;
}

bool Generator::attempt(Board & b, int r, int c)
{
    // Go to next square
    Board::increment(r, c);
    if (r >= Board::SIZE)
        return true;

    Board::ValueList possibleValues = b.allPossible(r, c);
    std::random_shuffle(possibleValues.begin(), possibleValues.end());
    for (auto x : possibleValues)
    {
        b.set(r, c, x);
        if (attempt(b, r, c))
            return true;
    }
    b.set(r, c, 0);
    return false;
}

std::vector<int> Generator::randomizedSquares()
{
    std::vector<int> squares(Board::SIZE * Board::SIZE);
    std::iota(squares.begin(), squares.end(), 0);
    std::random_shuffle(squares.begin(), squares.end());
    return squares;
}
