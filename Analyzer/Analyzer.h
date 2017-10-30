#pragma once

#include <string>
#include <vector>

#include "Board/Board.h"

class Analyzer
{
public:

    struct Step
    {
        enum Type { SOLVE, ELIMINATE, STUCK, DONE };
        enum ReasonId
        {
            NONE,
            allOtherNumbersTaken,
            onlySquareInRowForThisValue,
            onlySquareInBoxForThisValue
        };
        Type             type;
        std::vector<int> indexes;
        int              value;
        std::string      reason;
    };

    Analyzer(Board const & board);

    // Determines the next solution step
    Step next();

    // Returns the current state of the board
    Board const & board() const { return board_; }

    // Returns true if the analyzer can make no more progress
    bool done() const  { return done_; }

private:

    void solve(int r, int c, int x);
    void eliminate(std::vector<int> const & indexes, int x);
    bool allOtherNumbersTaken(int * r, int * c, int * x);
    bool onlySquareInRowForThisValue(int * r, int * c, int * x);
    bool onlySquareInColumnForThisValue(int * r, int * c, int * x);
    bool onlySquareInBoxForThisValue(int * r, int * c, int * x);

    Board board_;
    std::vector<int> unsolved_;         // Indexes of unsolved squares
    std::vector<int> possibilities_;    // Masks of possible values for each square
    bool done_;
};
