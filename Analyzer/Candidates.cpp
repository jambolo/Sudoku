#include "Candidates.h"

#include "Board/Board.h"
#include <cassert>
#include <vector>

int Candidates::value(Candidates::Type candidates)
{
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
        {
            assert(candidates == 1);
            return v;
        }
    }
    return 0;
}

std::vector<int> Candidates::values(Candidates::Type candidates)
{
    std::vector<int> values;
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
            values.push_back(v);
    }
    return values;
}

int Candidates::count(Candidates::Type candidates)
{
    int count = 0;
    while (candidates != 0)
    {
        candidates >>= 1;
        if (candidates & 1)
            ++count;
    }
    return count;
}

std::vector<int> Candidates::findAll(List const & candidates, int v)
{
    std::vector<int> found;
    for (int i = 0; i < Board::SIZE * Board::SIZE; ++i)
    {
        Type c = candidates[i];
        if (!solved(c) && (c & fromValue(v)))
            found.push_back(i);
    }
    return found;
}
