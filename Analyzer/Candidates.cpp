#include "Candidates.h"

#include "Board/Board.h"
#include <cassert>
#include <vector>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

int Candidates::value(Candidates::Type candidates)
{
    XCODE_COMPATIBLE_ASSERT(candidates < (1 << 10));
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
        {
            XCODE_COMPATIBLE_ASSERT(candidates == 1);
            return v;
        }
    }
    return 0;
}

std::vector<int> Candidates::values(Candidates::Type candidates)
{
    XCODE_COMPATIBLE_ASSERT(candidates < (1 << 10));
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
    XCODE_COMPATIBLE_ASSERT(candidates < (1 << 10));
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
    Type mask = fromValue(v);
    std::vector<int> found;
    for (int i = 0; i < candidates.size(); ++i)
    {
        Type c = candidates[i];
        if ((c & mask) && !isSolved(c))
            found.push_back(i);
    }
    return found;
}

std::vector<int> Candidates::findAll(List const & candidates, std::vector<int> const & group, int v)
{
    XCODE_COMPATIBLE_ASSERT(candidates.size() == Board::NUM_CELLS);
    XCODE_COMPATIBLE_ASSERT(group.size() == Board::SIZE);

    Type mask = fromValue(v);
    std::vector<int> found;
    for (int u = 0; u < Board::SIZE; ++u)
    {
        int  i = group[u];
        Type c = candidates[i];
        if ((c & mask) && !isSolved(c))
            found.push_back(i);
    }
    return found;
}
