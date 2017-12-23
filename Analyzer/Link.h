#pragma once

#include "Candidates.h"
#include <vector>

namespace Link
{

struct Strong
{
    using List = std::vector<Strong>;

    int value;
    int i0;
    int i1;

    // Returns all strong links to cell i
    static List find(Candidates::List const & candidates, int i);

    // Returns all strong links in the given group
    static List find(Candidates::List const & candidates, std::vector<int> const & group);

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
    static List find(Candidates::List const & candidates, int u0, std::vector<int> const & group);
};

struct Weak
{
    using List = std::vector<Weak>;

    std::vector<int> indexes;
    int value;
};

} // namespace Link
