#include "Solver.h"

#include "Board/Board.h"

bool Solver::solve(Board & b)
{
    return search(b, 0, 0);
}

bool Solver::search(Board & b, int r, int c)
{
    if (b.nextEmpty(r, c))
    {
        Board::ValueList possible = b.allPossible(r, c);
        for (auto x : possible)
        {
            b.set(r, c, x);
            if (search(b, r, c))
                return true;
        }
        b.set(r, c, Board::EMPTY);
        return false;
    }
    else
    {
        return true;
    }
}

std::vector<Board> Solver::allSolutions(Board const & b)
{
    std::vector<Board> solutions;
    Board copy(b);
    searchAll(copy, 0, 0, solutions);
    return solutions;
}

void Solver::searchAll(Board & b, int r, int c, std::vector<Board> & solutions)
{
    if (b.nextEmpty(r, c))
    {
        Board::ValueList possible = b.allPossible(r, c);
        for (auto x : possible)
        {
            b.set(r, c, x);
            searchAll(b, r, c, solutions);
        }
        b.set(r, c, Board::EMPTY);
    }
    else
    {
        solutions.push_back(b);
    }
}

bool Solver::hasUniqueSolution(Board const & b)
{
    return allSolutions(b).size() == 1;
}
