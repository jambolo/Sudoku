#pragma once

#include <vector>

#include "Board/Board.h"

class Analyzer
{
public:

    struct Operation
    {
        enum Type { SOLVE, ELIMINATE, STUCK };
        Type type;
        std::vector<int> indexes;
        int value;
        std::string reason;
    };
    
    Analyzer(Board const & board);

    Operation nextOperation();
    void solve(int r, int c, int x);
    void eliminate(std::vector<int> const & indexes, int x);

private:
    std::vector<int> unsolved_;
    bool findObviousStep(int* r, int* c, int* x);

    int possible_[Board::SIZE * Board::SIZE];
};
