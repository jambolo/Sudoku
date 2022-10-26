#if !defined(ANALYZER_HIDDEN_H_INCLUDED)
#define ANALYZER_HIDDEN_H_INCLUDED 1
#pragma once

#include "Board/Board.h"
#include "Candidates.h"

#include <string>
#include <vector>

class Hidden
{
public:
    Hidden(Board const & board, Candidates::List const & candidates)
        : board_(board)
        , candidates_(candidates)
    {
    }

    // Returns true if a hidden single exists
    bool singleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

    // Returns true if a hidden pair exists
    bool pairExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

    // Returns true if a hidden triple exists
    bool tripleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

    // Returns true if a hidden quad exists
    bool quadExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    bool single(std::vector<int> const & indexes,
                std::vector<int> &       eliminatedIndexes,
                std::vector<int> &       eliminatedValues);
    bool pair(std::vector<int> const & indexes,
              std::vector<int> &       eliminatedIndexes,
              std::vector<int> &       eliminatedValues,
              std::vector<int> &       hiddenValues);
    bool triple(std::vector<int> const & indexes,
                std::vector<int> &       eliminatedIndexes,
                std::vector<int> &       eliminatedValues,
                std::vector<int> &       hiddenValues);
    bool quad(std::vector<int> const & indexes,
              std::vector<int> &       eliminatedIndexes,
              std::vector<int> &       eliminatedValues,
              std::vector<int> &       hiddenValues);

    Board const & board_;
    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_HIDDEN_H_INCLUDED)
