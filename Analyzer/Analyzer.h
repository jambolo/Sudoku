#pragma once

#include "Board/Board.h"
#include "Candidates.h"
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
            X_WING,
            Y_WING
        };

        ActionId action;
        std::vector<int>    indexes;
        std::vector<int>    values;
        TechniqueId         technique;
        std::string         reason;

        // Returns the name of the TechniqueId
        static char const * techniqueName(TechniqueId technique);
    };

    Analyzer(Board const & board, bool verbose = false);
    Analyzer(Board const & board, std::vector<unsigned> const & candidates, bool verbose = false);

    // Determines the next solution step
    Step next();

    // Returns the current state of the board
    Board const & board() const { return board_; }

    // Returns true if the analyzer can make no more progress
    bool done() const { return done_; }

    // Returns true if the analyzer can make no more progress because it is stuck
    bool stuck() const { return stuck_; }

    // Draws the board with candidates
    void drawPenciledBoard() const;

private:

    void solve(int i, int x);
    void eliminate(std::vector<int> const & indexes, int x);
    void eliminate(std::vector<int> const & indexes, std::vector<int> const & values);

    bool hiddenSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool hiddenSingle(std::vector<int> const & indexes,
                      std::vector<int> &       eliminatedIndexes,
                      std::vector<int> &       eliminatedValues) const;

    bool hiddenPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool hiddenPair(std::vector<int> const & indexes,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       hiddenValues) const;

    bool hiddenTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool hiddenTriple(std::vector<int> const & indexes,
                      std::vector<int> &       eliminatedIndexes,
                      std::vector<int> &       eliminatedValues,
                      std::vector<int> &       hiddenValues) const;

    bool hiddenQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool hiddenQuad(std::vector<int> const & indexes,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       hiddenValues) const;

    bool nakedSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool nakedSingle(std::vector<int> & indexes, std::vector<int> & values) const;

    bool nakedPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool nakedPair(std::vector<int> const & indexes,
                   std::vector<int> &       eliminatedIndexes,
                   std::vector<int> &       eliminatedValues,
                   std::vector<int> &       nakedIndexes) const;

    bool nakedTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool nakedTriple(std::vector<int> const & indexes,
                     std::vector<int> &       eliminatedIndexes,
                     std::vector<int> &       eliminatedValues,
                     std::vector<int> &       nakedIndexes) const;

    bool nakedQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool nakedQuad(std::vector<int> const & indexes,
                   std::vector<int> &       eliminatedIndexes,
                   std::vector<int> &       eliminatedValues,
                   std::vector<int> &       nakedIndexes) const;

    bool lockedCandidatesFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool lockedCandidates(std::vector<int> const & indexes1,
                          std::vector<int> const & indexes2,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues) const;

    bool xWingFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;
    bool xWingRow(int                      r0,
                  std::vector<int> const & row,
                  std::vector<int> &       eliminatedIndexes,
                  std::vector<int> &       eliminatedValues,
                  std::vector<int> &       pivots) const;
    bool xWingColumn(int                      c0,
                     std::vector<int> const & column,
                     std::vector<int> &       eliminatedIndexes,
                     std::vector<int> &       eliminatedValues,
                     std::vector<int> &       pivots) const;

    bool yWingFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const;

    // Returns all unsolved candidates in the cells specified by the indexes
    unsigned allCandidates(std::vector<int> const & indexes) const;

#if defined(_DEBUG)
    // Returns true if the candidates data is consistent with the solution
    bool candidatesAreValid();
#endif // defined(_DEBUG)

    Board board_;                  // Current state of the board
    bool verbose_;                 // If true, then generate explanations
    Candidates::List candidates_;  // Masks of possible values for each cell
    bool done_  = false;           // True if the analyzer can do no more, either because it is stumped or the board is solved
    bool stuck_ = false;           // True if the analyzer can do no more, either because it is stumped
#if defined(_DEBUG)
    Board solvedBoard_;            // All results are checked against this board
#endif // defined(_DEBUG)
};
