#if !defined(ANALYZER_XYWING_H_INCLUDED)
#define ANALYZER_XYWING_H_INCLUDED 1
#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <string>
#include <vector>

class XYWing
{
public:
    XYWing(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if a xy-wing exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    bool XYWing::existsAt(int                i0,
                          std::vector<int> & pivots,
                          std::vector<int> & pivotValues,
                          std::vector<int> & indexes,
                          std::vector<int> & values,
                          std::string &      reason);

    static std::string generateReason(std::vector<int> const & pivots,
                                      std::vector<int> const & values,
                                      std::vector<int> const & eliminated);

    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_XYWING_H_INCLUDED)
