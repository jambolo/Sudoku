#include "XWing.h"

#include "Candidates.h"
#include "Link.h"

#include "Board/Board.h"

#include <cassert>
#include <set>
#include <string>
#include <vector>

namespace
{
int findColumnsWithCandidates(int row, int value, std::vector<Candidates::Type> const & board, std::set<int> & columns)
{
    int count = 0;
    for (int c = 0; c < Board::SIZE; ++c)
    {
        Candidates::Type candidates = board[Board::Cell::indexOf(row, c)];
        if (Candidates::includes(candidates, value) && !Candidates::isSolved(candidates))
        {
            columns.insert(c);
            ++count;
        }
    }
    return count;
}

int findRowsWithCandidates(int column, int value, std::vector<Candidates::Type> const & board, std::set<int> & rows)
{
    int count = 0;
    for (int r = 0; r < Board::SIZE; ++r)
    {
        Candidates::Type candidates = board[Board::Cell::indexOf(r, column)];
        if (Candidates::includes(candidates, value) && !Candidates::isSolved(candidates))
        {
            rows.insert(r);
            ++count;
        }
    }
    return count;
}
} // anonymous namespace

bool XWing::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // If there are exactly three rows (or columns) with a candidate value in either two or three columns (or rows), and those
    // colums (or rows) are the same in each of the rows (or columns), then the candidate cannot be in those three columns (or rows)
    // in any other rows (or columns)

    int eliminatedValue;
    std::vector<int> pivots;

    // Check for row-oriented
    eliminatedValue = findRow(indexes, pivots);
    if (eliminatedValue != Board::EMPTY)
    {
        values.push_back(eliminatedValue);
        reason = rowReason(eliminatedValue, pivots);
        return true;
    }

    // Check for column-oriented
    eliminatedValue = findColumn(indexes, pivots);
    if (eliminatedValue != Board::EMPTY)
    {
        values.push_back(eliminatedValue);
        reason = columnReason(eliminatedValue, pivots);
        return true;
    }

    return false;
}

int XWing::findRow(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots)
{
    for (int v = 1; v <= 9; ++v)
    {
        for (int r0 = 0; r0 < Board::SIZE - 1; ++r0)
        {
            std::set<int> columns0;
            int           count0 = findColumnsWithCandidates(r0, v, candidates_, columns0);
            if (count0 != 2 || columns0.size() != 2)
                continue;
            for (int r1 = r0 + 1; r1 < Board::SIZE; ++r1)
            {
                std::set<int> columns1 = columns0;
                int           count1   = findColumnsWithCandidates(r1, v, candidates_, columns1);
                if (count1 != 2 || columns1.size() != 2)
                    continue;

                // Found one!

                // Find any candidates to eliminate
                for (int r = 0; r < Board::SIZE; ++r)
                {
                    if (r != r0 && r != r1)
                    {
                        for (auto c : columns1)
                        {
                            int i = Board::Cell::indexOf(r, c);
                            if (Candidates::includes(candidates_[i], v))
                                eliminatedIndexes.push_back(i);
                        }
                    }
                }

                // If there are any candidates to eliminate then return them. Otherwise, keep looking.
                if (!eliminatedIndexes.empty())
                {
                    // Return the pivot indexes
                    for (auto r : { r0, r1 })
                    {
                        for (auto c : columns1)
                        {
                            pivots.push_back(Board::Cell::indexOf(r, c));
                        }
                    }

                    // Return the value to be eliminated
                    return v;
                }
            }
        }
    }
    return Board::EMPTY;
}

int XWing::findColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots)
{
    for (int v = 1; v <= 9; ++v)
    {
        for (int c0 = 0; c0 < Board::SIZE - 1; ++c0)
        {
            std::set<int> rows0;
            int           count0 = findRowsWithCandidates(c0, v, candidates_, rows0);
            if (count0 != 2 || rows0.size() != 2)
                continue;
            for (int c1 = c0 + 1; c1 < Board::SIZE; ++c1)
            {
                std::set<int> rows1  = rows0;
                int           count1 = findRowsWithCandidates(c1, v, candidates_, rows1);
                if (count1 != 2 || rows1.size() != 2)
                    continue;

                // Found one!

                // Find any candidates to eliminate
                for (int c = 0; c < Board::SIZE; ++c)
                {
                    if (c != c0 && c != c1)
                    {
                        for (auto r : rows1)
                        {
                            int i = Board::Cell::indexOf(r, c);
                            if (Candidates::includes(candidates_[i], v))
                                eliminatedIndexes.push_back(i);
                        }
                    }
                }

                // If there are any candidates to eliminate then return them. Otherwise, keep looking.
                if (!eliminatedIndexes.empty())
                {
                    // Return the pivot indexes
                    for (auto c : { c0, c1 })
                    {
                        for (auto r : rows1)
                        {
                            pivots.push_back(Board::Cell::indexOf(r, c));
                        }
                    }

                    // Return the value to be eliminated
                    return v;
                }
            }
        }
    }
    return Board::EMPTY;
}

std::string XWing::rowReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 4);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r3, c3;
    Board::Cell::locationOf(pivots[3], &r3, &c3);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) + " and " + Board::Cell::name(pivots[1]) +
                         " in row " + Board::Group::rowName(r0) +
                         " and " + Board::Cell::name(pivots[2]) + " and " + Board::Cell::name(pivots[3]) +
                         " in row " + Board::Group::rowName(r3) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same two columns, " +
                         Board::Group::columnName(c0) + " and " + Board::Group::columnName(c3) +
                         ". One of these squares in each columns must have this value and so the other squares in"
                         " these columns cannot.";
    return reason;
}

std::string XWing::columnReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 4);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r3, c3;
    Board::Cell::locationOf(pivots[3], &r3, &c3);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) + " and " + Board::Cell::name(pivots[1]) +
                         " in column " + Board::Group::columnName(c0) +
                         " and " + Board::Cell::name(pivots[2]) + " and " + Board::Cell::name(pivots[3]) +
                         " in column " + Board::Group::columnName(c3) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same two rows, " +
                         Board::Group::rowName(r0) + " and " + Board::Group::rowName(r3) +
                         ". One of these squares in each row must have this value and so the other squares in"
                         " these rows cannot.";
    return reason;
}

bool Swordfish::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // If there are exactly three rows (or columns) with a candidate value in either two or three columns (or rows), and those
    // colums (or rows) are the same in each of the rows (or columns), then the candidate cannot be in those three columns (or rows)
    // in any other rows (or columns)

    int eliminatedValue;
    std::vector<int> pivots;

    // Check for row-oriented
    eliminatedValue = findRow(indexes, pivots);
    if (eliminatedValue != Board::EMPTY)
    {
        values.push_back(eliminatedValue);
        reason = rowReason(eliminatedValue, pivots);
        return true;
    }

    // Check for column-oriented
    eliminatedValue = findColumn(indexes, pivots);
    if (eliminatedValue != Board::EMPTY)
    {
        values.push_back(eliminatedValue);
        reason = columnReason(eliminatedValue, pivots);
        return true;
    }

    return false;
}

int Swordfish::findRow(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots)
{
    for (int v = 1; v <= 9; ++v)
    {
        for (int r0 = 0; r0 < Board::SIZE - 2; ++r0)
        {
            std::set<int> columns0;
            int           count0 = findColumnsWithCandidates(r0, v, candidates_, columns0);
            if (count0 < 2 || columns0.size() > 3)
                continue;
            for (int r1 = r0 + 1; r1 < Board::SIZE - 1; ++r1)
            {
                std::set<int> columns1 = columns0;
                int           count1   = findColumnsWithCandidates(r1, v, candidates_, columns1);
                if (count1 < 2 || columns1.size() > 3)
                    continue;
                for (int r2 = r1 + 1; r2 < Board::SIZE; ++r2)
                {
                    std::set<int> columns2 = columns1;
                    int           count2   = findColumnsWithCandidates(r2, v, candidates_, columns2);
                    if (count2 < 2 || columns2.size() > 3)
                        continue;

                    // Found one!

                    // Find any candidates to eliminate
                    for (int r = 0; r < Board::SIZE; ++r)
                    {
                        if (r != r0 && r != r1 && r != r2)
                        {
                            for (auto c : columns2)
                            {
                                int i = Board::Cell::indexOf(r, c);
                                if (Candidates::includes(candidates_[i], v))
                                    eliminatedIndexes.push_back(i);
                            }
                        }
                    }

                    // If there are any candidates to eliminate then return them. Otherwise, keep looking.
                    if (!eliminatedIndexes.empty())
                    {
                        // Return the pivot indexes
                        for (auto r : { r0, r1, r2 })
                        {
                            for (auto c : columns2)
                            {
                                pivots.push_back(Board::Cell::indexOf(r, c));
                            }
                        }

                        // Return the value to be eliminated
                        return v;
                    }
                }
            }
        }
    }
    return Board::EMPTY;
}

int Swordfish::findColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots)
{
    for (int v = 1; v <= 9; ++v)
    {
        for (int c0 = 0; c0 < Board::SIZE - 2; ++c0)
        {
            std::set<int> rows0;
            int           count0 = findRowsWithCandidates(c0, v, candidates_, rows0);
            if (count0 < 2 || rows0.size() > 3)
                continue;
            for (int c1 = c0 + 1; c1 < Board::SIZE - 1; ++c1)
            {
                std::set<int> rows1  = rows0;
                int           count1 = findRowsWithCandidates(c1, v, candidates_, rows1);
                if (count1 < 2 || rows1.size() > 3)
                    continue;
                for (int c2 = c1 + 1; c2 < Board::SIZE; ++c2)
                {
                    std::set<int> rows2  = rows1;
                    int           count2 = findRowsWithCandidates(c2, v, candidates_, rows2);
                    if (count2 < 2 || rows2.size() > 3)
                        continue;

                    // Found one!

                    // Find any candidates to eiminate
                    for (int c = 0; c < Board::SIZE; ++c)
                    {
                        if (c != c0 && c != c1 && c != c2)
                        {
                            for (auto r : rows2)
                            {
                                int i = Board::Cell::indexOf(r, c);
                                if (Candidates::includes(candidates_[i], v))
                                    eliminatedIndexes.push_back(i);
                            }
                        }
                    }

                    // If there are any candidates to eliminate then return them. Otherwise, keep looking.
                    if (!eliminatedIndexes.empty())
                    {
                        // Return the pivot indexes
                        for (auto c : { c0, c1, c2 })
                        {
                            for (auto r : rows2)
                            {
                                pivots.push_back(Board::Cell::indexOf(r, c));
                            }
                        }

                        // Return the value to be eliminated
                        return v;
                    }
                }
            }
        }
    }
    return Board::EMPTY;
}

std::string Swordfish::rowReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 9);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r4, c4;
    Board::Cell::locationOf(pivots[4], &r4, &c4);
    int r8, c8;
    Board::Cell::locationOf(pivots[8], &r8, &c8);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) +
                         ", " + Board::Cell::name(pivots[1]) +
                         ", and " + Board::Cell::name(pivots[2]) +
                         " in row " + Board::Group::rowName(r0) +
                         ", " + Board::Cell::name(pivots[3]) +
                         ", " + Board::Cell::name(pivots[4]) +
                         ", and " + Board::Cell::name(pivots[5]) +
                         " in row " + Board::Group::rowName(r4) +
                         ", and " +
                         Board::Cell::name(pivots[6]) +
                         ", " + Board::Cell::name(pivots[7]) +
                         ", and " + Board::Cell::name(pivots[8]) +
                         " in row " + Board::Group::rowName(r8) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same three columns, " +
                         Board::Group::columnName(c0) +
                         ", " + Board::Group::columnName(c4) +
                         ", and " + Board::Group::columnName(c8) +
                         ". One of these squares in each column must have this value and so the other squares in"
                         " these columns cannot.";
    return reason;
}

std::string Swordfish::columnReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 9);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r4, c4;
    Board::Cell::locationOf(pivots[4], &r4, &c4);
    int r8, c8;
    Board::Cell::locationOf(pivots[8], &r8, &c8);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) +
                         ", " + Board::Cell::name(pivots[1]) +
                         ", and " + Board::Cell::name(pivots[2]) +
                         " in column " + Board::Group::columnName(c0) +
                         ", " + Board::Cell::name(pivots[3]) +
                         ", " + Board::Cell::name(pivots[4]) +
                         ", and " + Board::Cell::name(pivots[5]) +
                         " in column " + Board::Group::columnName(c4) +
                         ", and " + Board::Cell::name(pivots[6]) +
                         ", " + Board::Cell::name(pivots[7]) +
                         ", and " + Board::Cell::name(pivots[8]) +
                         " in column " + Board::Group::columnName(c8) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same three rows, " +
                         Board::Group::rowName(r0) +
                         ", " + Board::Group::rowName(r4) +
                         ", and " + Board::Group::rowName(r8) +
                         ". One of these squares in each row must have this value and so the other squares in"
                         " these rows cannot.";
    return reason;
}

bool Jellyfish::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // If there are exactly four rows (or columns) with a candidate value in either two, three or four columns (or
    // rows), and those colums (or rows) are the same in each of the rows (or columns), then the candidate cannot
    // be in those four columns (or rows) in any other rows (or columns)

    int eliminatedValue;
    std::vector<int> pivots;

    // Check for row-oriented
    eliminatedValue = findRow(indexes, pivots);
    if (eliminatedValue != Board::EMPTY)
    {
        values.push_back(eliminatedValue);
        reason = rowReason(eliminatedValue, pivots);
        return true;
    }

    // Check for column-oriented
    eliminatedValue = findColumn(indexes, pivots);
    if (eliminatedValue != Board::EMPTY)
    {
        values.push_back(eliminatedValue);
        reason = columnReason(eliminatedValue, pivots);
        return true;
    }

    return false;
}

int Jellyfish::findRow(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots)
{
    for (int v = 1; v <= 9; ++v)
    {
        for (int r0 = 0; r0 < Board::SIZE - 3; ++r0)
        {
            std::set<int> columns0;
            int           count0 = findColumnsWithCandidates(r0, v, candidates_, columns0);
            if (count0 < 2 || columns0.size() > 4)
                continue;
            for (int r1 = r0 + 1; r1 < Board::SIZE - 2; ++r1)
            {
                std::set<int> columns1 = columns0;
                int           count1   = findColumnsWithCandidates(r1, v, candidates_, columns1);
                if (count1 < 2 || columns1.size() > 4)
                    continue;
                for (int r2 = r1 + 1; r2 < Board::SIZE - 1; ++r2)
                {
                    std::set<int> columns2 = columns1;
                    int           count2   = findColumnsWithCandidates(r2, v, candidates_, columns2);
                    if (count2 < 2 || columns2.size() > 4)
                        continue;
                    for (int r3 = r2 + 1; r3 < Board::SIZE; ++r3)
                    {
                        std::set<int> columns3 = columns2;
                        int           count3   = findColumnsWithCandidates(r3, v, candidates_, columns3);
                        if (count3 < 2 || columns3.size() != 4)
                            continue;

                        // Found one!

                        // Find any candidates to eliminate
                        for (int r = 0; r < Board::SIZE; ++r)
                        {
                            if (r != r0 && r != r1 && r != r2 && r != r3)
                            {
                                for (auto c : columns3)
                                {
                                    int i = Board::Cell::indexOf(r, c);
                                    if (Candidates::includes(candidates_[i], v))
                                        eliminatedIndexes.push_back(i);
                                }
                            }
                        }

                        // If there are any candidates to eliminate then return them. Otherwise, keep looking.
                        if (!eliminatedIndexes.empty())
                        {
                            // Return the pivot indexes
                            for (auto r : { r0, r1, r2, r3 })
                            {
                                for (auto c : columns3)
                                {
                                    pivots.push_back(Board::Cell::indexOf(r, c));
                                }
                            }

                            // Return the value to be eliminated
                            return v;
                        }
                    }
                }
            }
        }
    }
    return Board::EMPTY;
}

int Jellyfish::findColumn(std::vector<int> & eliminatedIndexes, std::vector<int> & pivots)
{
    for (int v = 1; v <= 9; ++v)
    {
        for (int c0 = 0; c0 < Board::SIZE - 3; ++c0)
        {
            std::set<int> rows0;
            int           count0 = findRowsWithCandidates(c0, v, candidates_, rows0);
            if (count0 < 2 || rows0.size() > 4)
                continue;
            for (int c1 = c0 + 1; c1 < Board::SIZE - 2; ++c1)
            {
                std::set<int> rows1  = rows0;
                int           count1 = findRowsWithCandidates(c1, v, candidates_, rows1);
                if (count1 < 2 || rows1.size() > 4)
                    continue;
                for (int c2 = c1 + 1; c2 < Board::SIZE - 1; ++c2)
                {
                    std::set<int> rows2  = rows1;
                    int           count2 = findRowsWithCandidates(c2, v, candidates_, rows2);
                    if (count2 < 2 || rows2.size() > 4)
                        continue;
                    for (int c3 = c2 + 1; c3 < Board::SIZE; ++c3)
                    {
                        std::set<int> rows3  = rows2;
                        int           count3 = findRowsWithCandidates(c3, v, candidates_, rows3);
                        if (count3 < 2 || rows3.size() != 4)
                            continue;

                        // Found one!

                        // Find any candidates to eliminate
                        for (int c = 0; c < Board::SIZE; ++c)
                        {
                            if (c != c0 && c != c1 && c != c2 && c != c3)
                            {
                                for (auto r : rows2)
                                {
                                    int i = Board::Cell::indexOf(r, c);
                                    if (Candidates::includes(candidates_[i], v))
                                        eliminatedIndexes.push_back(i);
                                }
                            }
                        }

                        // If there are any candidates to eliminate then return them. Otherwise, keep looking.
                        if (!eliminatedIndexes.empty())
                        {
                            // Return the pivot indexes
                            for (auto c : { c0, c1, c2, c3 })
                            {
                                for (auto r : rows3)
                                {
                                    pivots.push_back(Board::Cell::indexOf(r, c));
                                }
                            }

                            // Return the value to be eliminated
                            return v;
                        }
                    }
                }
            }
        }
    }
    return Board::EMPTY;
}

std::string Jellyfish::rowReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 16);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r5, c5;
    Board::Cell::locationOf(pivots[5], &r5, &c5);
    int r10, c10;
    Board::Cell::locationOf(pivots[10], &r10, &c10);
    int r15, c15;
    Board::Cell::locationOf(pivots[15], &r15, &c15);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) +
                         ", " + Board::Cell::name(pivots[1]) +
                         ", " + Board::Cell::name(pivots[2]) +
                         ", and " + Board::Cell::name(pivots[3]) +
                         " in row " + Board::Group::rowName(r0) +
                         ", " + Board::Cell::name(pivots[4]) +
                         ", " + Board::Cell::name(pivots[5]) +
                         ", " + Board::Cell::name(pivots[6]) +
                         ", and " + Board::Cell::name(pivots[7]) +
                         " in row " + Board::Group::rowName(r5) +
                         ", " + Board::Cell::name(pivots[8]) +
                         ", " + Board::Cell::name(pivots[9]) +
                         ", " + Board::Cell::name(pivots[10]) +
                         ", and " + Board::Cell::name(pivots[11]) +
                         " in row " + Board::Group::rowName(r10) +
                         ", and " + Board::Cell::name(pivots[12]) +
                         ", " + Board::Cell::name(pivots[13]) +
                         ", " + Board::Cell::name(pivots[14]) +
                         ", and " + Board::Cell::name(pivots[15]) +
                         " in row " + Board::Group::rowName(r15) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same four columns, " +
                         Board::Group::columnName(c0) +
                         ", " + Board::Group::columnName(c5) +
                         ", " + Board::Group::columnName(c10) +
                         ", and " + Board::Group::columnName(c15) +
                         ", so one of the squares in each column must have this value and so other squares in"
                         " these columns cannot.";
    return reason;
}

std::string Jellyfish::columnReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 16);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r5, c5;
    Board::Cell::locationOf(pivots[5], &r5, &c5);
    int r10, c10;
    Board::Cell::locationOf(pivots[10], &r10, &c10);
    int r15, c15;
    Board::Cell::locationOf(pivots[15], &r15, &c15);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) +
                         ", " + Board::Cell::name(pivots[1]) +
                         ", " + Board::Cell::name(pivots[2]) +
                         ", and " + Board::Cell::name(pivots[3]) +
                         " in column " + Board::Group::columnName(c0) +
                         ", " + Board::Cell::name(pivots[4]) +
                         ", " + Board::Cell::name(pivots[5]) +
                         ", " + Board::Cell::name(pivots[6]) +
                         ", and " + Board::Cell::name(pivots[7]) +
                         " in column " + Board::Group::columnName(c5) +
                         ", " + Board::Cell::name(pivots[8]) +
                         ", " + Board::Cell::name(pivots[9]) +
                         ", " + Board::Cell::name(pivots[10]) +
                         ", and " + Board::Cell::name(pivots[11]) +
                         " in column " + Board::Group::columnName(c10) +
                         ", and " + Board::Cell::name(pivots[12]) +
                         ", " + Board::Cell::name(pivots[13]) +
                         ", " + Board::Cell::name(pivots[14]) +
                         ", and " + Board::Cell::name(pivots[15]) +
                         " in column " + Board::Group::columnName(c15) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same four rows, " +
                         Board::Group::rowName(r0) +
                         ", " + Board::Group::rowName(r5) +
                         ", " + Board::Group::rowName(r10) +
                         ", and " + Board::Group::rowName(r15) +
                         ", so one of the squares in each row must have this value and so other squares in"
                         " these rows cannot.";
    return reason;
}
