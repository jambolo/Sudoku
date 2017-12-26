#pragma once

#include <vector>

class Candidates
{
public:
    using Type = unsigned;
    using List = std::vector<Type>;

    static Type constexpr ALL = 0x3fe;

    // Returns true if there is only one candidate
    static bool solved(Type candidates)
    {
        return (candidates & (candidates - 1)) == 0;
    }

    // Returns true if there are exactly two candidates
    static bool biValue(Type candidates)
    {
        return count(candidates) == 2;
    }

    // Returns the value of the only candidate
    static int value(Type candidates);

    // Returns a list of of candidates
    static std::vector<int> values(Type candidates);

    // Returns the number of candidates
    static int count(Type candidates);

    // Returns the candidate corresponding to the value
    static Type fromValue(int v)
    {
        return 1 << v;
    }

    // Finds all unsolved cells with v as a candidate
    static std::vector<int> findAll(List const & candidates, int v);
};
