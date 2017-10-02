#include "Solver.h"

#include "Board/Board.h"

bool Solver::solve(Board & b)
{
    return search(b, 0, 0);
}

bool Solver::search(Board & board, int r, int c)
{
    if (board.nextEmpty(&r, &c))
    {
        Board::ValueList possible = board.allPossible(r, c);
        for (auto x : possible)
        {
            board.set(r, c, x);
            if (search(board, r, c))
                return true;
        }
        board.set(r, c, Board::EMPTY);
        return false;
    }
    else
    {
        return true;
    }
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
    if (board.nextEmpty(&r, &c))
    {
        Board::ValueList possible = board.allPossible(r, c);
        for (auto x : possible)
        {
            board.set(r, c, x);
            searchAll(board, r, c, solutions);
        }
        board.set(r, c, Board::EMPTY);
    }
    else
    {
        solutions.push_back(board);
    }
}

bool Solver::hasUniqueSolution(Board const & board)
{
    return allSolutions(board).size() == 1;
}
