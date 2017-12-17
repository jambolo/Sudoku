#pragma once

#include "Board/Board.h"
#include "Candidates.h"

#include <string>
#include <vector>

class Hidden
{
public:

    static bool singleExists(Board const &            board,
                             Candidates::List const & candidates,
                             std::vector<int> &       indexes,
                             std::vector<int> &       values,
                             std::string &            reason);

    static bool pairExists(Board const &            board,
                           Candidates::List const & candidates,
                           std::vector<int> &       indexes,
                           std::vector<int> &       values,
                           std::string &            reason);

    static bool tripleExists(Board const &            board,
                             Candidates::List const & candidates,
                             std::vector<int> &       indexes,
                             std::vector<int> &       values,
                             std::string &            reason);

    static bool quadExists(Board const &            board,
                           Candidates::List const & candidates,
                           std::vector<int> &       indexes,
                           std::vector<int> &       values,
                           std::string &            reason);

private:
    static bool single(Board const &            board,
                       Candidates::List const & candidates,
                       std::vector<int> const & indexes,
                       std::vector<int> &       eliminatedIndexes,
                       std::vector<int> &       eliminatedValues);
    static bool pair(Candidates::List const & candidates,
                     std::vector<int> const & indexes,
                     std::vector<int> &       eliminatedIndexes,
                     std::vector<int> &       eliminatedValues,
                     std::vector<int> &       hiddenValues);
    static bool triple(Candidates::List const & candidates,
                       std::vector<int> const & indexes,
                       std::vector<int> &       eliminatedIndexes,
                       std::vector<int> &       eliminatedValues,
                       std::vector<int> &       hiddenValues);
    static bool quad(Candidates::List const & candidates,
                     std::vector<int> const & indexes,
                     std::vector<int> &       eliminatedIndexes,
                     std::vector<int> &       eliminatedValues,
                     std::vector<int> &       hiddenValues);
};
