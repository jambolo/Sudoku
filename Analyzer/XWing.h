#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class XWing
{
public:
    static bool exists(Board const &            board,
                       Candidates::List const & candidates,
                       std::vector<int> &       indexes,
                       std::vector<int> &       values,
                       std::string &            reason);

private:
    static bool findRow(Candidates::List const & candidates,
                        int                      r0,
                        std::vector<int> const & row,
                        std::vector<int> &       eliminatedIndexes,
                        std::vector<int> &       eliminatedValues,
                        std::vector<int> &       pivots);
    static bool findColumn(Candidates::List const & candidates,
                           int                      c0,
                           std::vector<int> const & column,
                           std::vector<int> &       eliminatedIndexes,
                           std::vector<int> &       eliminatedValues,
                           std::vector<int> &       pivots);
    static std::string rowReason(int value, std::vector<int> const & pivots);
    static std::string columnReason(int value, std::vector<int> const & pivots);
};
