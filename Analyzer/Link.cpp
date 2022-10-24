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

bool Strong::isNormalized() const
{
    XCODE_COMPATIBLE_ASSERT(i0 != i1 || v0 != v1);  // Sanity check for invalid link
    return i0 < i1 || (i0 == i1 && v0 < v1);
}

void Strong::normalize()
{
    if (!isNormalized())
    {
        std::swap(i0, i1);
        std::swap(v0, v1);
    }
}

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

Strong::List Strong::Strong::find(Candidates::List const & candidates, int i, int v)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i >= 0 && i < Board::NUM_CELLS);

    List links;

    // Find the all strong links with the value v in the row containing square i
    std::vector<int> row = Board::Group::row(Board::Group::whichRow(i));
    List rowLinks        = find(candidates, i, v, row);
    links.insert(links.end(), rowLinks.begin(), rowLinks.end());

    // Find the all strong links in the column containing square i
    std::vector<int> column = Board::Group::column(Board::Group::whichColumn(i));
    List columnLinks        = find(candidates, i, v, column);
    links.insert(links.end(), columnLinks.begin(), columnLinks.end());

    // Find the all strong links in the box containing square i
    std::vector<int> box = Board::Group::box(Board::Group::whichBox(i));
    List boxLinks        = find(candidates, i, v, box);
    links.insert(links.end(), boxLinks.begin(), boxLinks.end());

    // Sort the list of links and remove duplicates
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
            Strong link{ values[0], values[1], i0, i0 };
            XCODE_COMPATIBLE_ASSERT(link.isNormalized());
            links.push_back(link);
        }

        // Ignore cells with only already-tested candidates
        candidates0 &= ~alreadyTested;
        if (!candidates0)
            continue;

        std::vector<int> values = Candidates::values(candidates0);
        for (auto v : values)
        {
            Candidates::Type c = Candidates::fromValue(v);
            for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
            {
                int i1 = group[u1];
                Candidates::Type candidates1 = candidates[i1];
                if (!Candidates::isSolved(candidates1) && (candidates0 & candidates1 & c))
                {
                    if (existsIncremental(candidates, u0, u1, c, group))
                    {
                        Strong link{ v, v, i0, i1 };
                        XCODE_COMPATIBLE_ASSERT(link.isNormalized());
                        links.push_back(link);
                    }
                    break; // These share a candidate, so more strong links for this value whether this is a strong link or not
                }
            }
        }
        alreadyTested |= candidates0;
    }
    return links;
}

Strong::List Strong::find(Candidates::List const & candidates, std::vector<int> const & group, int v)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    std::vector<int> indexes = Candidates::findAll(candidates, group, v);

    if (indexes.size() == 2)
    {
        Strong link{ v, v, indexes[0], indexes[1] };
        XCODE_COMPATIBLE_ASSERT(link.isNormalized());
        return List{ link };
    }
    else
    {
        return List();
    }
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
        Strong link{ values[0], values[1], i0, i0 };
        XCODE_COMPATIBLE_ASSERT(link.isNormalized());
        links.push_back(link);
    }

    for (auto v : values)
    {
        Candidates::Type mask = Candidates::fromValue(v);
        for (auto i1 : group)
        {
            if (i1 != i0 && exists(candidates, i0, i1, mask, group))
            {
                Strong link{ v, v, i0, i1 };
                link.normalize();
                links.push_back(link);
                break; // Only one strong link can exist in a group for any candidate
            }
        }
    }
    return links;
}

Strong::List Strong::find(Candidates::List const & candidates, int i0, int v, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    std::vector<int> indexes = Candidates::findAll(candidates, group, v);

    if (indexes.size() == 2)
    {
        XCODE_COMPATIBLE_ASSERT(indexes[0] == i0 || indexes[1] == i0);
        Strong link{ v, v, indexes[0], indexes[1] };
        XCODE_COMPATIBLE_ASSERT(link.isNormalized());
        return List{ link };
    }
    else
    {
        return List();
    }
}

/************************************************************************************************************************************/

bool Weak::isNormalized() const
{
    XCODE_COMPATIBLE_ASSERT(i0 != i1);  // Sanity check for invalid link
    return i0 < i1;
}

void Weak::normalize()
{
    if (!isNormalized())
        std::swap(i0, i1);
}

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

Weak::List Weak::find(Candidates::List const & candidates, int i, int v)
{
    XCODE_COMPATIBLE_ASSERT(i >= 0 && i < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(v >= 1 && v <= 9);

    List links;

    std::vector<int> row = Board::Group::row(Board::Group::whichRow(i));
    List rowLinks        = find(candidates, i, v, row);
    links.insert(links.end(), rowLinks.begin(), rowLinks.end());

    std::vector<int> column = Board::Group::column(Board::Group::whichColumn(i));
    List columnLinks        = find(candidates, i, v, column);
    links.insert(links.end(), columnLinks.begin(), columnLinks.end());

    std::vector<int> box = Board::Group::box(Board::Group::whichBox(i));
    List boxLinks        = find(candidates, i, v, box);
    links.insert(links.end(), boxLinks.begin(), boxLinks.end());

    std::sort(links.begin(), links.end());
    links.erase(std::unique(links.begin(), links.end()), links.end());

    return links;
}

Weak::List Weak::find(Candidates::List const & candidates, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
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
        for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
        {
            int i1 = group[u1];
            Candidates::Type candidates1 = candidates[i0];

            // Ignore solved cells
            if (Candidates::isSolved(candidates1))
                continue;

            for (auto v : values)
            {
                Candidates::Type c = Candidates::fromValue(v);
                if (candidates0 & candidates1 & c)
                    links.emplace_back(Weak{ v, i0, i1 });
            }
        }
    }

    // If there are less than two links, then it is a strong link or no link, and not a weak link
    return (links.size() > 1) ? links : List();
}

Weak::List Weak::find(Candidates::List const & candidates, std::vector<int> const & group, int v)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);
    XCODE_COMPATIBLE_ASSERT(v >= 1 && v <= 9);

    List links;

    Candidates::Type c = Candidates::fromValue(v);

    for (int u0 = 0; u0 < Board::SIZE - 1; ++u0)
    {
        int i0 = group[u0];
        Candidates::Type candidates0 = candidates[i0];

        // Ignore solved cells and cells without v
        if (Candidates::isSolved(candidates0) || (candidates0 & c) == Candidates::NONE)
            continue;

        std::vector<int> values = Candidates::values(candidates0);
        for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
        {
            int i1 = group[u1];
            Candidates::Type candidates1 = candidates[i0];

            if (!Candidates::isSolved(candidates1) && (candidates1 & c) != Candidates::NONE)
                links.emplace_back(Weak{ v, i0, i1 });
        }
    }

    // If there are less than two links, then it is a strong link or no link, and not a weak link
    return (links.size() > 1) ? links : List();
}

Weak::List Weak::find(Candidates::List const & candidates, int i0, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i0 >= 0 && i0 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(std::find(group.begin(), group.end(), i0) != group.end());
    XCODE_COMPATIBLE_ASSERT(!Candidates::isSolved(candidates[i0]));

    List links;

    Candidates::Type candidates0 = candidates[i0];
    std::vector<int> values      = Candidates::values(candidates0);

    for (int i1 : group)
    {
        if (i0 == i1)
            continue;

        Candidates::Type candidates1 = candidates[i1];

        for (auto v : values)
        {
            if (!Candidates::isSolved(candidates1) && Candidates::includes(candidates1, v))
            {
                Weak link{ v, i0, i1 };
                link.normalize();
                links.emplace_back(link);
            }
        }
    }

    // If there are less than two links, then it is a strong link or no link, and not a weak link
    return (links.size() > 1) ? links : List();
}

Weak::List Weak::find(Candidates::List const & candidates, int i0, int v, std::vector<int> const & group)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(i0 >= 0 && i0 < Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(v >= 1 && v < 9);
    XCODE_COMPATIBLE_ASSERT(std::find(group.begin(), group.end(), i0) != group.end());
    XCODE_COMPATIBLE_ASSERT(!Candidates::isSolved(candidates[i0]));
    XCODE_COMPATIBLE_ASSERT(Candidates::includes(candidates[i0], v));

    List links;

    Candidates::Type candidates0 = candidates[i0];
    std::vector<int> values      = Candidates::values(candidates0);

    for (int i1 : group)
    {
        if (i0 == i1)
            continue;

        Candidates::Type candidates1 = candidates[i1];

        if (!Candidates::isSolved(candidates1) && Candidates::includes(candidates1, v))
        {
            Weak link{ v, i0, i1 };
            link.normalize();
            links.emplace_back(link);
        }
    }
    return links;
}

bool Link::operator <(Strong const & lhs, Strong const & rhs)
{
    XCODE_COMPATIBLE_ASSERT(lhs.isNormalized());
    XCODE_COMPATIBLE_ASSERT(rhs.isNormalized());
    // Sort in order i0, i1, v0, v1
    if (lhs.i0 < rhs.i0)
        return true;
    if (rhs.i0 < lhs.i0)
        return false;
    if (lhs.i1 < rhs.i1)
        return true;
    if (rhs.i1 < lhs.i1)
        return false;
    if (lhs.v0 < rhs.v0)
        return true;
    if (rhs.v0 < lhs.v0)
        return false;
    return lhs.v1 < rhs.v1;
}

bool Link::operator ==(Strong const & lhs, Strong const & rhs)
{
    XCODE_COMPATIBLE_ASSERT(lhs.isNormalized());
    XCODE_COMPATIBLE_ASSERT(rhs.isNormalized());
    return (lhs.v0 == rhs.v0) &&
           (lhs.v1 == rhs.v1) &&
           (lhs.i0 == rhs.i0) &&
           (lhs.i1 == rhs.i1);
}

bool Link::operator <(Weak const & lhs, Weak const & rhs)
{
    XCODE_COMPATIBLE_ASSERT(lhs.isNormalized());
    XCODE_COMPATIBLE_ASSERT(rhs.isNormalized());
    // Sort in order i0, i1, v
    if (lhs.i0 < rhs.i0)
        return true;
    if (rhs.i0 < lhs.i0)
        return false;
    if (lhs.i1 < rhs.i1)
        return true;
    if (rhs.i1 < lhs.i1)
        return false;
    return lhs.v < rhs.v;
}

bool Link::operator ==(Weak const & lhs, Weak const & rhs)
{
    XCODE_COMPATIBLE_ASSERT(lhs.isNormalized());
    XCODE_COMPATIBLE_ASSERT(rhs.isNormalized());
    return (lhs.v == rhs.v) &&
           (lhs.i0 == rhs.i0) &&
           (lhs.i1 == rhs.i1);
}
