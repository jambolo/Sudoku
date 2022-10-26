#pragma once

#include "Board/Board.h"
#include "Candidates.h"
#include "Link.h"
#include <set>
#include <string>
#include <vector>

class XCycle
{
public:
    XCycle(Candidates::List const & candidates)
        : candidates_(candidates)
        , value_(0)
        , root_(0)
        , solves_(false)
    {
    }

    // Returns true if a X-Cycle elimination or solution exists
    bool exists(std::vector<int> & indexes, std::vector<int> & values, std::string & reason, bool & solves);

private:

    struct Reason1Dependency
    {
        int dependent;
        int weak0;
        int weak1;
    };

    // Recursively form a graph of alternating strong and any links by extending each strong link from the given index
    bool extendStrong(int from);

    // Recursively form a graph of alternating strong and any links by extending each link from the given index
    bool extendAny(int from);

    std::string generateReason1(std::vector<Reason1Dependency> const & dependencies);
    std::string generateReason2(int second, int last);
    std::string generateReason3(int last);

    Candidates::List const & candidates_;

    // Search context
    int value_;                     // The current value being analyzed
    int root_;                      // The current root index
    int solves_;                    // True if the found cycle solves a cell (rather than eliminating candidates)
    Link::Weak::List weakLinks_;    // All the weak links in the chain
    std::vector<int> chain_;        // Indexes in the chain (excluding the root index)

    // Solution values
    std::vector<int> indexes_;
    std::vector<int> values_;
    std::string reason_;
};
