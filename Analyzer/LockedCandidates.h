#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class LockedCandidates
{
public:
    static bool exists(Board const &            board,
                       Candidates::List const & candidates,
                       std::vector<int> &       indexes,
                       std::vector<int> &       values,
                       std::string &            reason);

private:
    static bool find(Candidates::List const & candidates,
                     std::vector<int> const & indexes1,
                     std::vector<int> const & indexes2,
                     std::vector<int> &       eliminatedIndexes,
                     std::vector<int> &       eliminatedValues);
    static std::string generateReason(std::string const & unit1,
                                      char                which1,
                                      std::string const & unit2,
                                      char                which2);

    // Returns all unsolved candidates in the cells specified by the indexes
    static Candidates::Type allCandidates(Candidates::List const & candidates, std::vector<int> const & indexes);
};
