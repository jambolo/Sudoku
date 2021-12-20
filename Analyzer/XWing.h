#if !defined(ANALYZER_XWING_H_INCLUDED)
#define ANALYZER_XWING_H_INCLUDED   1
#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class XWing
{
public:
    XWing(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if an x-2ing exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    bool findRow(int                      r0,
                 std::vector<int> const & row,
                 std::vector<int> &       eliminatedIndexes,
                 std::vector<int> &       eliminatedValues,
                 std::vector<int> &       pivots);
    bool findColumn(int                      c0,
                    std::vector<int> const & column,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       pivots);
    static std::string rowReason(int value, std::vector<int> const & pivots);
    static std::string columnReason(int value, std::vector<int> const & pivots);

    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_XWING_H_INCLUDED)
