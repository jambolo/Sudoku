#pragma once

#include "Candidates.h"
#include <string>
#include <vector>

class UniqueRectangle
{
public:
    UniqueRectangle(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if a unique rectangle pattern exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    bool exists(int                i1,
                int                i2,
                int                i3,
                int                i4,
                Candidates::Type   roofCandidates,
                std::vector<int> & indexes,
                std::vector<int> & values,
                std::string &      reason);

    std::string generateType1Reason(int                      floor0,
                                    int                      floor1,
                                    int                      roof0,
                                    int                      roof1,
                                    int                      index,
                                    std::vector<int> const & values);
    std::string generateType2Reason(int                      floor0,
                                    int                      floor1,
                                    int                      roof0,
                                    int                      roof1,
                                    std::vector<int> const & indexes,
                                    int                      value);

    Candidates::List const & candidates_;
};
