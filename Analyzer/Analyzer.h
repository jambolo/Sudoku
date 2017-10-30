#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Board/Board.h"

class Analyzer
{
public:

    struct Step
    {
        enum Type { SOLVE, ELIMINATE, STUCK, DONE };
        enum Reason
        {
            NONE = 0,
            NAKED_SINGLE,
            HIDDEN_SINGLE_ROW,
            HIDDEN_SINGLE_COLUMN,
            HIDDEN_SINGLE_BOX,
            NAKED_PAIR_ROW,
            NAKED_PAIR_COLUMN,
            NAKED_PAIR_BOX,
            NAKED_TRIPLE_ROW,
            NAKED_TRIPLE_COLUMN,
            NAKED_TRIPLE_BOX,
            NAKED_QUAD_ROW,
            NAKED_QUAD_COLUMN,
            NAKED_QUAD_BOX,
        };
        Type             type;
        std::vector<int> indexes;
        std::vector<int> values;
        Reason           reason;
        std::string      reasonText;
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
    bool nakedSingle(int * r, int * c, int * x);
    bool hiddenSingle(std::vector<int>const & indexes, int s, int * solvedR, int * solvedC, int * &solvedValue);
    bool hiddenSingleRow(int * r, int * c, int * x);
    bool hiddenSingleColumn(int * r, int * c, int * x);
    bool hiddenSingleBox(int * r, int * c, int * x);
    bool nakedPair(std::vector<int> const & indexes, std::vector<int> &eliminatedIndexes, std::vector<int> &eliminatedValues);
    bool nakedPairRow(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedPairColumn(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedPairBox(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedTripleRow(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedTripleColumn(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedTripleBox(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedQuadRow(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedQuadColumn(std::vector<int> & indexes, std::vector<int> & values);
    bool nakedQuadBox(std::vector<int> & indexes, std::vector<int> & values);

    Board board_;
    std::vector<int> unsolved_;         // Indexes of unsolved squares
    std::vector<unsigned> candidates_;  // Masks of possible values for each square
    bool done_;
};
