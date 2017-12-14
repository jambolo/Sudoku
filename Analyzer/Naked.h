#pragma once

#include "Candidates.h"
#include "Board/Board.h"
#include <vector>
#include <string>

class Naked
{
public:
    static bool singleExists(Board const & board,
        Candidates::List const & candidates,
        std::vector<int> & indexes, std::vector<int> & values,
        std::string & reason);
    static bool pairExists(Board const & board,
        Candidates::List const & candidates,
        std::vector<int> & indexes,
        std::vector<int> & values,
        std::string & reason);
    static bool tripleExists(Board const & board,
        Candidates::List const & candidates,
        std::vector<int> & indexes,
        std::vector<int> & values,
        std::string & reason);
    static bool quadExists(Board const & board,
        Candidates::List const & candidates,
        std::vector<int> & indexes,
        std::vector<int> & values,
        std::string & reason);

private:
    static bool single(Board const & board,
        Candidates::List const & candidates,
        std::vector<int> & indexes,
        std::vector<int> & values);
    static bool pair(Candidates::List const & candidates,
        std::vector<int> const & indexes,
        std::vector<int> &       eliminatedIndexes,
        std::vector<int> &       eliminatedValues,
        std::vector<int> &       nakedIndexes);
    static bool triple(Candidates::List const & candidates,
        std::vector<int> const & indexes,
        std::vector<int> &       eliminatedIndexes,
        std::vector<int> &       eliminatedValues,
        std::vector<int> &       nakedIndexes);
    static bool quad(Candidates::List const & candidates,
        std::vector<int> const & indexes,
        std::vector<int> &       eliminatedIndexes,
        std::vector<int> &       eliminatedValues,
        std::vector<int> &       nakedIndexes);

    static std::string pairReason(std::string const & unit, char which, std::vector<int> const & indexes);
    static std::string tripleReason(std::string const & unit, char which, std::vector<int> const & indexes);
    static std::string quadReason(std::string const & unit, char which, std::vector<int> const & indexes);
};
