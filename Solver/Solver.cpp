#include "Solver.h"

#include "Board/Board.h"

bool Solver::solve(Board & b)
{
    return search(b, 0, 0);
}

bool Solver::search(Board & board, int r, int c)
{
    // Find the next empty square. If there are none, then a solution has been found
    if (!board.nextEmpty(&r, &c))
        return true;

    std::vector<int> possible = board.allPossible(r, c);
    for (auto x : possible)
    {
        board.set(r, c, x);
        if (search(board, r, c))
            return true;
    }

    // No solution
    board.set(r, c, Board::EMPTY); // Reset for backtrack
    return false;
}

std::vector<Board> Solver::allSolutions(Board const & board)
{
    std::vector<Board> solutions;
    Board copy(board);
    searchAll(copy, 0, 0, solutions);
    return solutions;
}

void Solver::searchAll(Board & board, int r, int c, std::vector<Board> & solutions)
{
    // Find the next empty square. If there are none, then a solution has been found
    if (!board.nextEmpty(&r, &c))
    {
        solutions.push_back(board);
        return;
    }

    std::vector<int> possible = board.allPossible(r, c);
    for (auto x : possible)
    {
        board.set(r, c, x);
        searchAll(board, r, c, solutions);
    }

    board.set(r, c, Board::EMPTY); // Reset for backtrack
}

void Solver::searchUnique(Board & board, int r, int c, int & count)
{
    // Find the next empty square. If there are none, then a solution has been found
    if (!board.nextEmpty(&r, &c))
    {
        ++count;
        return;
    }

    // Try all the possible values for the empty square
    std::vector<int> possible = board.allPossible(r, c);
    for (auto x : possible)
    {
        board.set(r, c, x);
        searchUnique(board, r, c, count);
        if (count > 1)
            return; // Early out -- not unique
    }

    board.set(r, c, Board::EMPTY); // Reset for backtrack
}

bool Solver::hasUniqueSolution(Board const & board)
{
    Board copy(board);
    int count = 0;
    searchUnique(copy, 0, 0, count);
    return count == 1;
}
