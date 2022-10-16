#if !defined(ANALYZER_XWING_H_INCLUDED)
#define ANALYZER_XWING_H_INCLUDED 1
#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include <set>
#include <string>
#include <vector>

class XWing
{
public:
    XWing(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if an x-wing exists
    // Returns the indexes and values to eliminate and a description
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    int                findRow(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots);
    int                findColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots);
    static std::string rowReason(int value, std::vector<int> const & pivots);
    static std::string columnReason(int value, std::vector<int> const & pivots);

    Candidates::List const & candidates_;
};

class Swordfish
{
public:
    Swordfish(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if an swordfish exists
    // Returns the indexes and value to eliminate and a description
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    int                findRow(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots);
    int                findColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots);
    static std::string rowReason(int value, std::vector<int> const & pivots);
    static std::string columnReason(int value, std::vector<int> const & pivots);

    Candidates::List const & candidates_;
};

class Jellyfish
{
public:
    Jellyfish(Candidates::List const & candidates) : candidates_(candidates) {}

    // Returns true if an jellyfish exists
    // Returns the indexes and value to eliminate and a description
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason);

private:
    int findRow(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots);
    int findColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots);

    static std::string rowReason(int value, std::vector<int> const & pivots);
    static std::string columnReason(int value, std::vector<int> const & pivots);

    Candidates::List const & candidates_;
};

#endif // defined(ANALYZER_XWING_H_INCLUDED)
