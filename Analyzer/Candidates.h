#if !defined(ANALYZER_CANDIDATES_H_INCLUDED)
#define ANALYZER_CANDIDATES_H_INCLUDED 1
#pragma once

#include <cassert>
#include <vector>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

// Information and operations associated with the candidate values of a cell
class Candidates
{
public:
    using Type = unsigned;              // Storage type for storing candidates as a single value
    using List = std::vector<Type>;     // A list of candidates for multiple cells

    static Type constexpr ALL  = 0x3fe; // Value representing all candidates
    static Type constexpr NONE = 0;     // Value representing no candidates

    // Returns true if the given value is one of the candidates
    static bool includes(Type candidates, int value)
    {
        XCODE_COMPATIBLE_ASSERT(value >= 1 && value <= 9);
        return (candidates & (1 << value)) != 0;
    }

    // Returns true if there is only one candidate
    static bool isSolved(Type candidates)
    {
        XCODE_COMPATIBLE_ASSERT(candidates != NONE);
        return (candidates & (candidates - 1)) == 0;
    }

    // Returns true if there are exactly two candidates
    static bool isBivalue(Type candidates)
    {
        XCODE_COMPATIBLE_ASSERT(candidates != NONE);
        unsigned x = candidates & (candidates - 1); // Remove one candidate
        return (x != 0) && isSolved(x); // Return true if there is another
    }

    // Returns true if there are exactly three candidates
    static bool isTrivalue(Type candidates)
    {
        XCODE_COMPATIBLE_ASSERT(candidates != NONE);
        unsigned x = candidates & (candidates - 1); // Remove one candidate
        return (x != 0) && isBivalue(x); // Return true if the remainder is a bivalue
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
        XCODE_COMPATIBLE_ASSERT(v >= 1 && v <= 9);
        return 1 << v;
    }

    // Finds all unsolved cells with v as a candidate
    static std::vector<int> findAll(List const & candidates, int v);

    // Finds all unsolved cells in a group with v as a candidate
    static std::vector<int> findAll(List const& candidates, std::vector<int> const & group, int v);
};

#endif // defined(ANALYZER_CANDIDATES_H_INCLUDED)
