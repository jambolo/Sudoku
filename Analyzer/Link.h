#pragma once

#include "Candidates.h"
#include <vector>

namespace Link
{

    struct Strong
    {
        using List = std::vector<Strong>;

        int u0;
        int i0;
        int u1;
        int i1;
        int value;

        static List find(Candidates::List const & candidates, int i);
        static List find(Candidates::List const & candidates, std::vector<int> const & unit);
        static List find(Candidates::List const & candidates, int i, std::vector<int> const & unit);
        static bool exists(Candidates::List const & candidates,
            int i0,
            int i1,
            Candidates::Type mask,
            std::vector<int> const & unit);
        static bool existsIncremental(Candidates::List const & candidates,
            int u0,
            int u1,
            Candidates::Type mask,
            std::vector<int> const & unit);
    };

    struct Weak
    {
        using List = std::vector<Weak>;

        std::vector<int> indexes;
        int value;
    };


} // namespace Link
