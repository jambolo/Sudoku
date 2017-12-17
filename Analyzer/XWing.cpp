#include "XWing.h"

#include "Candidates.h"
#include "Link.h"

#include "Board/Board.h"

#include <cassert>
#include <string>
#include <vector>

bool XWing::exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // If a value is a candidate in a row (or column) in exactly two cells and it is a candidate in another row
    // (or column) in exactly the same two columns (or rows) as the first row (or or column), then it cannot be
    // a candidate in any other cells in those two columns (or rows).

    bool found;
    std::vector<int> pivots;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        return !findRow(r, row, indexes, values, pivots);
    });
    if (found)
    {
        reason = rowReason(values[0], pivots);
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        return !findColumn(c, column, indexes, values, pivots);
    });
    if (found)
    {
        reason = columnReason(values[0], pivots);
        return true;
    }

    return false;
}

bool XWing::findRow(int                      r0,
                    std::vector<int> const & row,
                    std::vector<int> &       eliminatedIndexes,
                    std::vector<int> &       eliminatedValues,
                    std::vector<int> &       pivots)
{
    Link::Strong::List links = Link::Strong::find(candidates_, row);
    for (auto link : links)
    {
        int c0 = link.u0;
        int c1 = link.u1;
        int v  = link.value;
        Candidates::Type mask = Candidates::fromValue(v);

        for (int r1 = r0 + 1; r1 < Board::SIZE; ++r1)
        {
            std::vector<int> otherRow = Board::Unit::row(r1);
            int other0 = otherRow[c0];
            int other1 = otherRow[c1];

            // If there is a strong link with the same value in the same columns in another row, then
            // this is an x-wing.
            if (Link::Strong::exists(candidates_, other0, other1, mask, otherRow))
            {
                // Look for other cells in these two columns with the value as a candidate
                for (int c : { c0, c1 })
                {
                    std::vector<int> column = Board::Unit::column(c);
                    Board::ForEach::indexExcept(column, column[r0], column[r1], [&](int i) {
                        if (mask & candidates_[i])
                            eliminatedIndexes.push_back(i);
                    });
                }
                if (!eliminatedIndexes.empty())
                {
                    eliminatedValues.push_back(v);
                    pivots =
                    {
                        Board::Cell::indexOf(r0, c0),
                        Board::Cell::indexOf(r0, c1),
                        Board::Cell::indexOf(r1, c0),
                        Board::Cell::indexOf(r1, c1)
                    };
                    return true;
                }
            }
        }
    }
    return false;
}

bool XWing::findColumn(int                      c0,
                       std::vector<int> const & column,
                       std::vector<int> &       eliminatedIndexes,
                       std::vector<int> &       eliminatedValues,
                       std::vector<int> &       pivots)
{
    Link::Strong::List links = Link::Strong::find(candidates_, column);
    for (auto link : links)
    {
        int r0 = link.u0;
        int r1 = link.u1;
        int v  = link.value;
        Candidates::Type mask = Candidates::fromValue(v);

        for (int c1 = c0 + 1; c1 < Board::SIZE; ++c1)
        {
            std::vector<int> otherColumn = Board::Unit::column(c1);
            int other0 = otherColumn[r0];
            int other1 = otherColumn[r1];

            // If there is a strong link with the same value in the same columns in another column, then
            // this is an x-wing.
            if (Link::Strong::exists(candidates_, other0, other1, mask, otherColumn))
            {
                // Look for other cells in these two rows with the value as a candidate
                for (int r : { r0, r1 })
                {
                    std::vector<int> row = Board::Unit::row(r);
                    Board::ForEach::indexExcept(row, row[c0], row[c1], [&](int i) {
                        if (mask & candidates_[i])
                            eliminatedIndexes.push_back(i);
                    });
                }
                if (!eliminatedIndexes.empty())
                {
                    eliminatedValues.push_back(v);
                    pivots =
                    {
                        Board::Cell::indexOf(r0, c0),
                        Board::Cell::indexOf(r0, c1),
                        Board::Cell::indexOf(r1, c0),
                        Board::Cell::indexOf(r1, c1)
                    };
                    return true;
                }
            }
        }
    }
    return false;
}

std::string XWing::rowReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 4);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r3, c3;
    Board::Cell::locationOf(pivots[3], &r3, &c3);

    std::string reason = "Only " +
                         Board::Cell::name(pivots[0]) +
                         " and " +
                         Board::Cell::name(pivots[1]) +
                         " in row " +
                         Board::Unit::rowName(r0) +
                         " and only " +
                         Board::Cell::name(pivots[2]) +
                         " and " +
                         Board::Cell::name(pivots[3]) +
                         " in row " +
                         Board::Unit::rowName(r3) +
                         " can have the value " +
                         std::to_string(value) +
                         ". These squares are in the same two columns, " +
                         Board::Unit::columnName(c0) +
                         " and " +
                         Board::Unit::columnName(c3) +
                         ", so one of the squares in each column must have this value and none of the other squares"
                         " in these columns can.";
    return reason;
}

std::string XWing::columnReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 4);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r3, c3;
    Board::Cell::locationOf(pivots[3], &r3, &c3);

    std::string reason = "Only " +
                         Board::Cell::name(pivots[0]) +
                         " and " +
                         Board::Cell::name(pivots[1]) +
                         " in column " +
                         Board::Unit::columnName(c0) +
                         " and only " +
                         Board::Cell::name(pivots[2]) +
                         " and " +
                         Board::Cell::name(pivots[3]) +
                         " in column " +
                         Board::Unit::columnName(c3) +
                         " can have the value " +
                         std::to_string(value) +
                         ". These squares are in the same two rows, " +
                         Board::Unit::rowName(r0) +
                         " and " +
                         Board::Unit::rowName(r3) +
                         ". One of the squares in each row must have this value and so none of the other squares in"
                         " these rows can.";
    return reason;
}
