#pragma once

#include <vector>

class Board;

class Solver
{
public:
    static bool solve(Board & board);
    static std::vector<Board> allSolutions(Board const & b);
    static bool hasUniqueSolution(Board const & b);

private:
    static bool search(Board &b, int r, int c);
    static void searchAll(Board & b, int r, int c, std::vector<Board> & solutions);
};
