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

    struct StrongLink
    {
        int u0;
        int i0;
        int u1;
        int i1;
        int value;
    };

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

    unsigned allCandidates(std::vector<int>::const_iterator first, std::vector<int>::const_iterator last) const;
    unsigned allUnsolvedCandidates(std::vector<int>::const_iterator first, std::vector<int>::const_iterator last) const;

    std::vector<StrongLink> findStrongLinks(int i) const;
    std::vector<StrongLink> findStrongLinks(int i, std::vector<int> const & unit) const;
    std::vector<Analyzer::StrongLink> findStrongLinks(std::vector<int> const & unit) const;
    bool hasStrongLink(int i0, int i1, unsigned mask, std::vector<int> const & unit) const;
    bool hasStrongLinkR(int u0, int u1, unsigned mask, std::vector<int> const & unit) const;

#if defined(_DEBUG)
    bool candidatesAreValid();
#endif // defined(_DEBUG)

    Board board_;                       // Current state of the board
    bool verbose_;                      // If true, then generate explanations
    std::vector<unsigned> candidates_;  // Masks of possible values for each cell
    bool done_  = false;                // True if the analyzer can do no more, either because it is stumped or the board is solved
    bool stuck_ = false;                // True if the analyzer can do no more, either because it is stumped

#if defined(_DEBUG)
    Board solvedBoard_;                 // All results are checked against this board
#endif // defined(_DEBUG)
};
