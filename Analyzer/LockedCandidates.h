#if !defined(ANALYZER_LOCKEDCANDIDATES_H_INCLUDED)
#define ANALYZER_LOCKEDCANDIDATES_H_INCLUDED 1
#pragma once

#include "Candidates.h"
#include <string>
#include <vector>

class LockedCandidates
{
public:
    LockedCandidates(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if locked candidates exist
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    bool find(std::vector<int> const & indexes1,
              std::vector<int> const & indexes2,
              std::vector<int> &       eliminatedIndexes,
              std::vector<int> &       eliminatedValues);
    static std::string generateReason(std::string const & group1, char which1, std::string const & group2, char which2);

    // Returns all unsolved candidates in the cells specified by the indexes
    Candidates::Type allCandidates(std::vector<int> const & indexes);

    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_LOCKEDCANDIDATES_H_INCLUDED)
