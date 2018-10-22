#include "Generator.h"

#include "Board/Board.h"
#include "Solver/Solver.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

Board Generator::generate(int difficulty /* = 0*/)
{
    if (difficulty <= 0)
        difficulty = Board::NUM_CELLS;

    // Generate a random solved board
    Board board = generateSolvedBoard();

    // Randomly remove as many cells as possible until no unique solution can be found
    std::vector<int> indexes = randomizedIndexes();
    for (int i : indexes)
    {
        // Try to remove a cell
        int x = board.get(i);
        board.set(i, Board::EMPTY);

        // If the new puzzle doesn't have a unique solution, then undo and try again
        if (!Solver::hasUniqueSolution(board))
        {
            board.set(i, x); // Skip this one
            continue;
        }

        // If enough values have been removed, then we are done
        if (--difficulty <= 0)
            break;
    }
    return board;
}

Board Generator::generateSolvedBoard()
{
    Board board;
    bool successful = attempt(board, 0);
    XCODE_COMPATIBLE_ASSERT(successful);
    return board;
}

bool Generator::attempt(Board & board, int i)
{
    // This function attempts to find a solution recursively by attempting all possible values for the specified cell in random order
    // until values can be found for all of the following cells.

    // If there are no remaining cells to try, then the board has been generated
    if (i >= Board::NUM_CELLS)
        return true;

    // Generate all the candidates for this cell. If there are none, then the board is not viable.
    std::vector<int> possibleValues = board.candidates(i);
    if (possibleValues.empty())
        return false;

    std::random_shuffle(possibleValues.begin(), possibleValues.end());
    for (int x : possibleValues)
    {
        board.set(i, x);

        // Try to fill in the remaining cells. If that succeeds, then the board has been generated
        if (attempt(board, i + 1))
            return true;
    }

    // None of the candidates for this cell worked, so it isn't viable
    board.set(i, Board::EMPTY); // Undo any attempt in this cell
    return false;
}

std::vector<int> Generator::randomizedIndexes()
{
    std::vector<int> indexes(Board::NUM_CELLS);
    std::iota(indexes.begin(), indexes.end(), 0);
    std::random_shuffle(indexes.begin(), indexes.end());
    return indexes;
}
