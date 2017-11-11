#pragma once

#include "Board/Board.h"
#include <functional>
#include <string>
#include <vector>

class Analyzer
{
public:

    struct Step
    {
        enum ActionId { SOLVE, ELIMINATE, STUCK, DONE };
        enum TechniqueId
        {
            NONE = 0,
            HIDDEN_SINGLE,
            HIDDEN_PAIR,
            HIDDEN_TRIPLE,
            HIDDEN_QUAD,
            NAKED_SINGLE,
            NAKED_PAIR,
            NAKED_TRIPLE,
            NAKED_QUAD,
        };
        ActionId         action;
        std::vector<int> indexes;
        std::vector<int> values;
        TechniqueId      technique;
        std::string      reason;
        static char const * techniqueName(TechniqueId technique);
    };

    Analyzer(Board const & board);

    // Determines the next solution step
    Step next();

    // Returns the current state of the board
    Board const & board() const { return board_; }

    // Returns true if the analyzer can make no more progress
    bool done() const { return done_; }

private:

    void solve(int r, int c, int x);
    void eliminate(std::vector<int> const & indexes, int x);
    void eliminate(std::vector<int> const & indexes, std::vector<int> const & values);
    
    bool hiddenSingleFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool hiddenSingle(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);
    
    bool hiddenPairFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool hiddenPair(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);
    
    bool hiddenTripleFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool hiddenTriple(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);
    
    bool hiddenQuadFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool hiddenQuad(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);
    
    bool nakedSingleFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool nakedSingle(std::vector<int> & indexes, std::vector<int> & values);
    
    bool nakedPairFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool nakedPair(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);
    
    bool nakedTripleFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool nakedTriple(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);
    
    bool nakedQuadFound(std::vector<int> & indexes, std::vector<int> & values, char const ** details);
    bool nakedQuad(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);

    Board board_;                       // Current state of the board
    std::vector<int> unsolved_;         // Indexes of unsolved squares
    std::vector<unsigned> candidates_;  // Masks of possible values for each square
    bool done_;                         // True if the analyzer can do no more, either because it is stumped or the board is solved
};
