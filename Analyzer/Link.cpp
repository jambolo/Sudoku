#include "Link.h"

#include "Board/Board.h"
#include "Candidates.h"

#include <algorithm>
#include <cassert>
#include <vector>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

using namespace Link;

Strong::List Strong::find(Candidates::List const & candidates, int i)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i >= 0 && i < Board::NUM_CELLS);

    List links;

    // Find the all strong links in the row containing square i
    std::vector<int> row = Board::Group::row(Board::Group::whichRow(i));
    List rowLinks        = find(candidates, i, row);
    links.insert(links.end(), rowLinks.begin(), rowLinks.end());

    // Find the all strong links in the column containing square i
    std::vector<int> column = Board::Group::column(Board::Group::whichColumn(i));
    List columnLinks        = find(candidates, i, column);
    links.insert(links.end(), columnLinks.begin(), columnLinks.end());

    // Find the all strong links in the box containing square i
    std::vector<int> box = Board::Group::box(Board::Group::whichBox(i));
    List boxLinks        = find(candidates, i, box);
    links.insert(links.end(), boxLinks.begin(), boxLinks.end());

    std::sort(links.begin(), links.end());
    links.erase(std::unique(links.begin(), links.end()), links.end());

    return links;
}

Strong::List Strong::find(Candidates::List const & candidates, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);
    List links;

    Candidates::Type alreadyTested = 0;
    for (int u0 = 0; u0 < Board::SIZE - 1; ++u0)
    {
        int i0 = group[u0];
        Candidates::Type candidates0 = candidates[i0];

        // Ignore solved cells
        if (Candidates::isSolved(candidates0))
            continue;

        // If this is a bivalue, then there is an intracell link from one candidate to the other
        if (Candidates::isBivalue(candidates0))
        {
            std::vector<int> values = Candidates::values(candidates0);
            XCODE_COMPATIBLE_ASSERT(values.size() == 2);
            links.emplace_back(Strong{ values[0], values[1], i0, i0 });
        }

        // Ignore cells with only already-tested candidates
        candidates0 &= ~alreadyTested;
        if (!candidates0)
            continue;

        std::vector<int> values = Candidates::values(candidates0);
        for (auto v : values)
        {
            Candidates::Type mask = Candidates::fromValue(v);
            for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
            {
                int i1 = group[u1];
                Candidates::Type candidates1 = candidates[i1];
                if (!Candidates::isSolved(candidates1) && (candidates0 & candidates1 & mask))
                {
                    if (existsIncremental(candidates, u0, u1, mask, group))
                        links.emplace_back(Strong{ v, v, i0, i1 });
                    break; // These share a candidate, so more strong links for this value whether this is a strong link or not
                }
            }
        }
        alreadyTested |= candidates0;
    }
    return links;
}

Strong::List Strong::find(Candidates::List const & candidates, int i0, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(i0 >= 0 && i0 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(std::find(group.begin(), group.end(), i0) != group.end());

    List links;
    Candidates::Type candidates0 = candidates[i0];
    std::vector<int> values      = Candidates::values(candidates0);

    // If this is a bivalue, then there is an intracell link from one candidate to the other
    if (Candidates::isBivalue(candidates0))
    {
        XCODE_COMPATIBLE_ASSERT(values.size() == 2);
        links.emplace_back(Strong{ values[0], values[1], i0, i0 });
    }

    for (auto v : values)
    {
        Candidates::Type mask = Candidates::fromValue(v);
        for (auto i1 : group)
        {
            if (i1 != i0 && exists(candidates, i0, i1, mask, group))
            {
                links.emplace_back(Strong{ v, v, i0, i1 });
                break; // Only one strong link can exist in a group for any candidate
            }
        }
    }
    return links;
}

bool Strong::exists(Candidates::List const & candidates,
                    int                      i0,
                    int                      i1,
                    Candidates::Type         mask,
                    std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(i0 >= 0 && i0 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i1 >= 0 && i1 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i0 != i1);
    XCODE_COMPATIBLE_ASSERT(std::find(group.begin(), group.end(), i0) != group.end());
    XCODE_COMPATIBLE_ASSERT(std::find(group.begin(), group.end(), i1) != group.end());
    XCODE_COMPATIBLE_ASSERT(Candidates::isSolved(mask));    // Only one candidate at a time

    // Not a strong link if the two cells don't share the candidate
    if (!(candidates[i0] & candidates[i1] & mask))
        return false;

    // Not a strong link if any other cells in the group share the candidate
    for (auto i : group)
    {
        if (i != i0 && i != i1 && (candidates[i] & mask))
            return false;
    }
    return true;
}

bool Strong::existsIncremental(Candidates::List const & candidates,
                               int                      u0,
                               int                      u1,
                               Candidates::Type         mask,
                               std::vector<int> const & group)
{
    // This is a faster version of exists(). This one requires that u0 < u1, and that no other cells in the given group in the range
    // [0, u1) have candidates corresponding to mask.
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);
    XCODE_COMPATIBLE_ASSERT(u0 >= 0 && u0 < Board::SIZE);
    XCODE_COMPATIBLE_ASSERT(u1 >= 0 && u1 < Board::SIZE);
    XCODE_COMPATIBLE_ASSERT(u0 < u1);
    XCODE_COMPATIBLE_ASSERT(mask != 0);

#if defined(_DEBUG)
    {
        Candidates::Type check = 0;
        for (int t = 0; t < u1; ++t)
        {
            if (t != u0)
                check |= candidates[group[t]];
        }
        XCODE_COMPATIBLE_ASSERT(!(check & mask));
    }
#endif // if defined(_DEBUG)

    // Not a strong link if the two cells don't share the candidate
    if ((candidates[group[u0]] & candidates[group[u1]] & mask) == 0)
        return false;

    // Not a strong link if any of the remaining cells in the group share the candidate
    for (int u2 = u1 + 1; u2 < Board::SIZE; ++u2)
    {
        int i2 = group[u2];
        if ((candidates[i2] & mask))
            return false;
    }
    return true;
}

/************************************************************************************************************************************/

Weak::List Weak::find(Candidates::List const & candidates, int i)
{
    XCODE_COMPATIBLE_ASSERT(i >= 0 && i < Board::NUM_CELLS);

    List links;

    std::vector<int> row = Board::Group::row(Board::Group::whichRow(i));
    List rowLinks        = find(candidates, i, row);
    links.insert(links.end(), rowLinks.begin(), rowLinks.end());

    std::vector<int> column = Board::Group::column(Board::Group::whichColumn(i));
    List columnLinks        = find(candidates, i, column);
    links.insert(links.end(), columnLinks.begin(), columnLinks.end());

    std::vector<int> box = Board::Group::box(Board::Group::whichBox(i));
    List boxLinks        = find(candidates, i, box);
    links.insert(links.end(), boxLinks.begin(), boxLinks.end());

    std::sort(links.begin(), links.end());
    links.erase(std::unique(links.begin(), links.end()), links.end());

    return links;
}

Weak::List Weak::find(Candidates::List const & candidates, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    List links;

    for (int u0 = 0; u0 < Board::SIZE - 1; ++u0)
    {
        int i0 = group[u0];
        Candidates::Type candidates0 = candidates[i0];

        // Ignore solved cells
        if (Candidates::isSolved(candidates0))
            continue;

        std::vector<int> values = Candidates::values(candidates0);
        for (auto v : values)
        {
            Candidates::Type mask = Candidates::fromValue(v);
            for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
            {
                int i1 = group[u1];
                if (!Candidates::isSolved(candidates[i1]) && exists(candidates, i0, i1, mask))
                    links.emplace_back(Weak{ v, i0, i1 });
            }
        }
    }
    return links;
}

Weak::List Weak::find(Candidates::List const & candidates, int i0, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(i0 >= 0 && i0 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(std::find(group.begin(), group.end(), i0) != group.end());

    List links;
    std::vector<int> values = Candidates::values(candidates[i0]);
    for (auto v : values)
    {
        Candidates::Type mask = Candidates::fromValue(v);
        for (int i1 : group)
        {
            if (i1 != i0 && !Candidates::isSolved(candidates[i1]) && exists(candidates, i0, i1, mask))
                links.emplace_back(Weak{ v, i0, i1 });
        }
    }
    return links;
}

bool Weak::exists(Candidates::List const & candidates, int i0, int i1, Candidates::Type mask)
{
    XCODE_COMPATIBLE_ASSERT(i0 >= 0 && i0 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i1 >= 0 && i1 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i0 != i1);
    XCODE_COMPATIBLE_ASSERT(mask != 0);

    return (candidates[i0] & candidates[i1] & mask) != 0;
}

bool Link::operator <(Strong const & lhs, Strong const & rhs)
{
    if (lhs == rhs)
        return false;
    if (lhs.v0 < rhs.v0)
        return true;
    if (rhs.v0 < lhs.v0)
        return false;
    if (lhs.v1 < rhs.v1)
        return true;
    if (rhs.v1 < lhs.v1)
        return false;
    if (lhs.i0 < rhs.i0)
        return true;
    if (rhs.i0 < lhs.i0)
        return false;
    return lhs.i1 < rhs.i1;
}

bool Link::operator ==(Strong const & lhs, Strong const & rhs)
{
    return (lhs.v0 == rhs.v0) &&
           (lhs.v1 == rhs.v1) &&
           (((lhs.i0 == rhs.i0) && (lhs.i1 == rhs.i1)) || ((lhs.i0 == rhs.i1) && (lhs.i1 == rhs.i0)));
}

bool Link::operator <(Weak const & lhs, Weak const & rhs)
{
    XCODE_COMPATIBLE_ASSERT(lhs.i0 != lhs.i1);
    XCODE_COMPATIBLE_ASSERT(rhs.i0 != rhs.i1);

    if (lhs.v0 < rhs.v0)
        return true;
    if (rhs.v0 < lhs.v0)
        return false;
    if (std::min(lhs.i0, lhs.i1) < std::min(rhs.i0, rhs.i1))
        return true;
    if (std::min(lhs.i0, lhs.i1) > std::min(rhs.i0, rhs.i1))
        return false;
    return std::max(lhs.i0, lhs.i1) < std::max(rhs.i0, rhs.i1);
}

bool Link::operator ==(Weak const & lhs, Weak const & rhs)
{
    XCODE_COMPATIBLE_ASSERT(lhs.i0 != lhs.i1);
    XCODE_COMPATIBLE_ASSERT(rhs.i0 != rhs.i1);

    return (lhs.v0 == rhs.v0) &&
           ((lhs.i0 == rhs.i0) || (lhs.i0 == rhs.i1)) &&
           ((lhs.i1 == rhs.i1) || (lhs.i1 == rhs.i0));
}
