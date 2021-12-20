#if !defined(SOLVER_SOLVER_H_INCLUDED)
#define SOLVER_SOLVER_H_INCLUDED 1
#pragma once

#include <vector>

class Board;

class Solver
{
public:
    static bool               solve(Board & board);
    static std::vector<Board> allSolutions(Board const & b);
    static bool               hasUniqueSolution(Board const & b);

private:
    static bool search(Board & b, int r, int c);
    static void searchAll(Board & b, int r, int c, std::vector<Board> & solutions);
    static void searchUnique(Board & board, int r, int c, int & count);
};

#endif // defined(SOLVER_SOLVER_H_INCLUDED)
