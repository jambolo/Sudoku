#if !defined(ANALYZER_LINK_H_INCLUDED)
#define ANALYZER_LINK_H_INCLUDED 1
#pragma once

#include "Candidates.h"
#include <vector>

namespace Link
{
// A strong link is a link such that (*i0 == v0) ^ (*i1 == v1) in the solution. Note that if i0 == i1, then v0 != v1 and the cell
// is a bi-value.
class Strong
{
public:
    using List = std::vector<Strong>;

    int v0; // Candidate value at i0
    int v1; // Candidate value at i1
    int i0; // Index of one node in the link
    int i1; // Index of the other node in the link

    // Returns true if the link is normalized, i.e. i0 < i1 or if equal then v0 < v1
    bool isNormalized() const;

    // Normalizes the link if not normalized
    void normalize();

    // Returns all strong links to cell i
    static List find(Candidates::List const & candidates, int i);

    // Returns all strong links to cell i with the value v
    static List find(Candidates::List const & candidates, int i, int v);

    // Returns all strong links in the given group
    static List find(Candidates::List const & candidates, std::vector<int> const & group);

    // Returns a strong link with the value v in the group if one exists, or an empty list
    static List find(Candidates::List const & candidates, std::vector<int> const & group, int v);

    // Returns true if a strong link exists between the two cells for the given value
    static bool exists(Candidates::List const & candidates,
                       int                      i0,
                       int                      i1,
                       Candidates::Type         mask,
                       std::vector<int> const & group);

    // Returns true if a strong link exists between the two cells for the given value
    static bool existsIncremental(Candidates::List const & candidates,
                                  int                      u0,
                                  int                      u1,
                                  Candidates::Type         mask,
                                  std::vector<int> const & group);
private:
    // Returns all the strong links at i0 in the group
    static List find(Candidates::List const & candidates, int i0, std::vector<int> const & group);

    // Returns a strong link at i0 with the value v in the group if one exists, or an empty list
    static List find(Candidates::List const & candidates, int i0, int v, std::vector<int> const & group);
};

// A weak link is such that (*i0 != v) | (*i1 != v) in the solution. Note that i0 != i1.
class Weak
{
public:
    using List = std::vector<Weak>;

    int v; // Value in common
    int i0; // Index of one node in the link
    int i1; // Index of the other node in the link

    // Returns true if the link is normalized, i.e. i0 < i1 or if equal then v0 < v1
    bool isNormalized() const;

    // Normalizes the link if not normalized
    void normalize();

    // Returns all weak links to cell i
    static List find(Candidates::List const & candidates, int i);

    // Returns all weak links to cell i with the value v
    static List find(Candidates::List const & candidates, int i, int v);

    // Returns all weak links in the given group
    static List find(Candidates::List const & candidates, std::vector<int> const & group);

    // Returns all weak links with the value v in the given group
    static List find(Candidates::List const & candidates, std::vector<int> const & group, int v);

private:
    // Returns all weak links in the given group with i0 at one end
    static List find(Candidates::List const & candidates, int i0, std::vector<int> const & group);

    // Returns all weak links in the given group with the value v and i0 at one end
    static List find(Candidates::List const & candidates, int i0, int v, std::vector<int> const & group);
};

bool operator <(Strong const & lhs, Strong const & rhs);
bool operator ==(Strong const & lhs, Strong const & rhs);
bool operator <(Weak const & lhs, Weak const & rhs);
bool operator ==(Weak const & lhs, Weak const & rhs);
} // namespace Link

#endif // defined(ANALYZER_LINK_H_INCLUDED)
