#if !defined(ANALYZER_NAKED_H_INCLUDED)
#define ANALYZER_NAKED_H_INCLUDED 1
#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class Naked
{
public:
    Naked(Board const & board, Candidates::List const & candidates) : board_(board), candidates_(candidates) {}

    // Returns true if a naked single exists
    bool singleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

    // Returns true if a naked pair exists
    bool pairExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

    // Returns true if a naked triple exists
    bool tripleExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

    // Returns true if a naked quad exists
    bool quadExists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    bool single(std::vector<int> & indexes, std::vector<int> & values);
    bool pair(std::vector<int> const & indexes,
              std::vector<int> &       eliminatedIndexes,
              std::vector<int> &       eliminatedValues,
              std::vector<int> &       nakedIndexes);
    bool triple(std::vector<int> const & indexes,
                std::vector<int> &       eliminatedIndexes,
                std::vector<int> &       eliminatedValues,
                std::vector<int> &       nakedIndexes);
    bool quad(std::vector<int> const & indexes,
              std::vector<int> &       eliminatedIndexes,
              std::vector<int> &       eliminatedValues,
              std::vector<int> &       nakedIndexes);

    static std::string pairReason(std::string const & group, char which, std::vector<int> const & indexes);
    static std::string tripleReason(std::string const & group, char which, std::vector<int> const & indexes);
    static std::string quadReason(std::string const & group, char which, std::vector<int> const & indexes);

    Board const & board_;
    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_NAKED_H_INCLUDED)
