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
            LOCKED_CANDIDATES,
            X_WING
        };

        ActionId action;
        std::vector<int>    indexes;
        std::vector<int>    values;
        TechniqueId         technique;
        std::string         reason;
        static char const * techniqueName(TechniqueId technique);
    };

    Analyzer(Board const & board, bool verbose = false);

    // Determines the next solution step
    Step next();

    // Returns the current state of the board
    Board const & board() const { return board_; }

    // Returns true if the analyzer can make no more progress
    bool done() const { return done_; }

    // Returns true if the analyzer can make no more progress because it is stuck
    bool stuck() const { return stuck_; }

    void drawPenciledBoard() const;

private:

    void solve(int r, int c, int x);
    void eliminate(std::vector<int> const & indexes, int x);
    void eliminate(std::vector<int> const & indexes, std::vector<int> const & values);

    bool hiddenSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool hiddenSingle(std::vector<int> const & indexes, std::vector<int> & eliminatedIndexes, std::vector<int> & eliminatedValues);

    bool hiddenPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool hiddenPair(std::vector<int> const & indexes,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       hiddenValues);

    bool hiddenTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool hiddenTriple(std::vector<int> const & indexes,
                      std::vector<int> &       eliminatedIndexes,
                      std::vector<int> &       eliminatedValues,
                      std::vector<int> &       hiddenValues);

    bool hiddenQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool hiddenQuad(std::vector<int> const & indexes,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       hiddenValues);

    bool nakedSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool nakedSingle(std::vector<int> & indexes, std::vector<int> & values);

    bool nakedPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool nakedPair(std::vector<int> const & indexes,
                   std::vector<int> &       eliminatedIndexes,
                   std::vector<int> &       eliminatedValues,
                   std::vector<int> &       nakedIndexes);

    bool nakedTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool nakedTriple(std::vector<int> const & indexes,
                     std::vector<int> &       eliminatedIndexes,
                     std::vector<int> &       eliminatedValues,
                     std::vector<int> &       nakedIndexes);

    bool nakedQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool nakedQuad(std::vector<int> const & indexes,
                   std::vector<int> &       eliminatedIndexes,
                   std::vector<int> &       eliminatedValues,
                   std::vector<int> &       nakedIndexes);

    bool lockedCandidatesFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool lockedCandidates(std::vector<int> const & indexes1,
                          std::vector<int> const & indexes2,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues);

    bool xWingFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);
    bool xWing(std::vector<int> const & indexes,
        std::vector<int> & eliminatedIndexes,
        std::vector<int> & eliminatedValues,
        int * otherUnit,
        std::vector<int> pivots);

    Board board_;                       // Current state of the board
    bool verbose_;                      // If true, then generate explanations
    std::vector<int> unsolved_;         // Indexes of unsolved squares
    std::vector<unsigned> candidates_;  // Masks of possible values for each square
    bool done_;                         // True if the analyzer can do no more, either because it is stumped or the board is solved
    bool stuck_;                        // True if the analyzer can do no more, either because it is stumped
};
