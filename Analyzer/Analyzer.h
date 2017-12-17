#pragma once

#include "Board/Board.h"
#include "Candidates.h"
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
            LOCKED_CANDIDATES,
            X_WING,
            Y_WING,
            SIMPLE_COLORING
        };

        ActionId         action;
        std::vector<int> indexes;
        std::vector<int> values;
        TechniqueId      technique;
        std::string      reason;

        // Returns the name of the TechniqueId
        static char const * techniqueName(TechniqueId technique);
    };

    Analyzer(Board const & board);
    Analyzer(Board const & board, Candidates::List const & candidates);

    // Determines the next solution step
    Step next();

    // Returns the current state of the board
    Board const & board() const { return board_; }

    // Returns true if the analyzer can make no more progress
    bool done() const { return done_; }

    // Returns true if the analyzer can make no more progress because it is stuck
    bool stuck() const { return stuck_; }

    // Draws a board with candidates
    void drawCandidates() const;

private:

    void solve(int i, int x);
    void eliminate(std::vector<int> const & indexes, int x);
    void eliminate(std::vector<int> const & indexes, std::vector<int> const & values);

#if defined(_DEBUG)
    // Returns true if the candidates data is consistent with the solution
    bool candidatesAreValid();
#endif // defined(_DEBUG)

    Board board_;                  // Current state of the board
    Candidates::List candidates_;  // Masks of possible values for each cell
    bool done_  = false;           // True if the analyzer can do no more, either because it is stumped or the board is solved
    bool stuck_ = false;           // True if the analyzer can do no more, either because it is stumped
#if defined(_DEBUG)
    Board solvedBoard_;            // All results are checked against this board
#endif // defined(_DEBUG)
};
