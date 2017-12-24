#include "Generator.h"

#include "Board/Board.h"
#include "Solver/Solver.h"

#include <algorithm>
#include <numeric>
#include <vector>

Board Generator::generate(int difficulty /* = 0*/)
{
    if (difficulty <= 0)
        difficulty = Board::NUM_CELLS;

    // Generate a random solved board
    Board board;
    attempt(board);

    // Randomly remove as many cells as possible until no unique solution can be found
    std::vector<int> indexes = randomizedIndexes();
    for (int i : indexes)
    {
        int r = i / Board::SIZE;
        int c = i % Board::SIZE;
        int x = board.get(r, c);
        board.set(r, c, Board::EMPTY);
        if (Solver::hasUniqueSolution(board))
        {
            if (--difficulty <= 0)
                return board;
        }
        else
        {
            board.set(r, c, x); // Skip this one
        }
    }
    return board;
}

bool Generator::attempt(Board & board, int r /* = 0*/, int c /* = -1*/)
{
    // Go to next cell
    Board::Cell::next(&r, &c);
    if (r >= Board::SIZE)
        return true;

    std::vector<int> possibleValues = board.candidates(r, c);
    std::random_shuffle(possibleValues.begin(), possibleValues.end());
    for (int x : possibleValues)
    {
        board.set(r, c, x);
        if (attempt(board, r, c))
            return true;
    }
    board.set(r, c, 0);
    return false;
}

std::vector<int> Generator::randomizedIndexes()
{
    std::vector<int> indexes(Board::NUM_CELLS);
    std::iota(indexes.begin(), indexes.end(), 0);
    std::random_shuffle(indexes.begin(), indexes.end());
    return indexes;
}
