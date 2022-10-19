#if !defined(ANALYZER_ANALYZER_H_INCLUDED)
#define ANALYZER_ANALYZER_H_INCLUDED 1
#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// Given a board and optionally with pre-determined candidates, an Analyzer will solve the puzzle a step at a time.
class Analyzer
{
public:
    // Information about a solution step.
    struct Step
    {
        // The potential actions for a step.
        enum ActionId
        {
            SOLVE,      // The value of a square is determined.
            ELIMINATE,  // One or more candiates for one or more squares have been eliminated.
            STUCK,      // The analyzer cannot determine the next step in a solution.
            DONE        // The puzzle is solved there are no more steps.
        };
        static int constexpr NUMBER_OF_ACTIONS = ActionId::DONE - ActionId::SOLVE + 1;  // Number of possible actions

        //! A technique's ID.
        enum TechniqueId
        {
            NONE = 0,           // No technique was used.
            NAKED_SINGLE,
            NAKED_PAIR,
            NAKED_TRIPLE,
            NAKED_QUAD,
            HIDDEN_SINGLE,
            HIDDEN_PAIR,
            HIDDEN_TRIPLE,
            HIDDEN_QUAD,
            LOCKED_CANDIDATES,
            X_WING,
            SWORDFISH,
            JELLYFISH,
            XY_WING,
            SIMPLE_COLORING,
            UNIQUE_RECTANGLE,
            LAST = UNIQUE_RECTANGLE
        };
        static int constexpr NUMBER_OF_TECHNIQUES = TechniqueId::LAST - TechniqueId::NONE + 1;

        ActionId action;            // Action perforned in the step
        TechniqueId technique;      // Technique used
        std::vector<int> indexes;   // Affected indexes
        std::vector<int> values;    // Affected values
        std::string reason;         // Explanation of the step

        // Returns the name of the TechniqueId
        static char const * techniqueName(TechniqueId technique);

        // Returns the name of the ActionId
        static char const * actionName(ActionId action);

        // Returns the difficulty factor of the technique
        static int techniqueDifficulty(TechniqueId technique);

        // Returns the value as a JSON object
        nlohmann::json toJson() const;
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

    // Returns the value as a JSON object
    nlohmann::json toJson() const;

private:
    void setValue(int i, int x);
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

#endif // defined(ANALYZER_ANALYZER_H_INCLUDED)
