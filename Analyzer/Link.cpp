#include "Link.h"

#include "Candidates.h"
#include "Board/Board.h"

#include <cassert>
#include <vector>

using namespace Link;

std::vector<Strong> Strong::find(Candidates::List const & candidates, int i)
{
    List links;

    std::vector<int> row = Board::Unit::row(Board::Unit::whichRow(i));
    List rowLinks = find(candidates, Board::Unit::offsetInRow(i), row);
    links.insert(links.end(), rowLinks.begin(), rowLinks.end());

    std::vector<int> column = Board::Unit::column(Board::Unit::whichColumn(i));
    List columnLinks = find(candidates, Board::Unit::offsetInColumn(i), column);
    links.insert(links.end(), columnLinks.begin(), columnLinks.end());

    std::vector<int> box = Board::Unit::box(Board::Unit::whichBox(i));
    List boxLinks = find(candidates, Board::Unit::offsetInBox(i), box);
    links.insert(links.end(), boxLinks.begin(), boxLinks.end());

    return links;
}

std::vector<Strong> Strong::find(Candidates::List const & candidates, std::vector<int> const & unit)
{
    List links;

    Candidates::Type alreadyTested = 0;
    for (int u0 = 0; u0 < Board::SIZE - 1; ++u0)
    {
        int i0 = unit[u0];
        Candidates::Type candidates0 = candidates[i0];

        // Ignore solved cells
        if (Candidates::solved(candidates0))
            continue;

        // Ignore cells with only already-tested candidate
        candidates0 &= ~alreadyTested;
        if (!candidates0)
            continue;

        std::vector<int> values = Candidates::values(candidates0);
        for (int v : values)
        {
            Candidates::Type mask = Candidates::fromValue(v);
            for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
            {
                int i1 = unit[u1];
                if ((candidates[i0] & candidates[i1] & mask))
                {
                    if (existsIncremental(candidates, u0, u1, mask, unit))
                        links.emplace_back(Strong{ u0, i0, u1, i1, v });
                    break;
                }
            }
        }
        alreadyTested |= candidates0;
    }
    return links;
}

std::vector<Strong> Strong::find(Candidates::List const & candidates, int u0, std::vector<int> const & unit)
{
    int i0 = unit[u0];
    std::vector<Strong> links;
    std::vector<int> values = Candidates::values(candidates[i0]);
    for (int v : values)
    {
        Candidates::Type mask = Candidates::fromValue(v);
        for (int u1 = 0; u1 < Board::SIZE; ++u1)
        {
            int i1 = unit[u1];
            if (i1 != i0)
            {
                if (exists(candidates, i0, i1, mask, unit))
                    links.emplace_back(Strong{ u0, i0, u1, i1, v });
            }
        };
    }
    return links;
}

bool Strong::exists(Candidates::List const & candidates,
                    int i0,
                    int i1,
                    Candidates::Type mask,
                    std::vector<int> const & unit)
{
    // Not a strong link if the two cells don't share the candidate
    if (!(candidates[i0] & candidates[i1] & mask))
        return false;

    // Not a strong link if any other cells in the unit share the candidate
    for (int i : unit)
    {
        if (i != i0 && i != i1 && (candidates[i] & mask))
            return false;
    }
    return true;
}

bool Strong::existsIncremental(Candidates::List const & candidates,
                               int u0,
                               int u1,
                               Candidates::Type mask,
                               std::vector<int> const & unit)
{
    // This is a faster version of exists(). This one requires that u0 < u1, and that no other cells in the given
    // unit in the range [0, u1) have candidates corresponding to mask.

#if defined(_DEBUG)
    {
        assert(u0 < u1);
        Candidates::Type check = 0;
        for (int t = 0; t < u1; ++t)
        {
            if (t != u0)
                check |= candidates[unit[t]];
        }
        assert(!(check & mask));
    }
#endif // if defined(_DEBUG)

    // Not a strong link if the two cells don't share the candidate
    if ((candidates[unit[u0]] & candidates[unit[u1]] & mask) == 0)
        return false;

    // Not a strong link if any of the remaining cells in the unit share the candidate
    for (int u2 = u1 + 1; u2 < Board::SIZE; ++u2)
    {
        int i2 = unit[u2];
        if ((candidates[i2] & mask))
            return false;
    }
    return true;
}
