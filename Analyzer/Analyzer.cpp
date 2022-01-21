#include "Analyzer.h"

#include "Candidates.h"
#include "Hidden.h"
#include "Link.h"
#include "LockedCandidates.h"
#include "Naked.h"
#include "SimpleColoring.h"
#include "XWing.h"
#include "XYWing.h"

#include "Board/Board.h"
#if defined(_DEBUG)
#include "Solver/Solver.h"
#endif // defined(_DEBUG)

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <algorithm>
#include <cassert>
#include <numeric>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

struct TechniqueInfoEntry
{
    char const * name;
    int difficulty;
};

// Info about techniques by technique ID
static TechniqueInfoEntry const TECHNIQUE_INFO[Analyzer::Step::NUMBER_OF_TECHNIQUES] =
{
    { "none",              0 },
    { "hidden single",     2 },
    { "hidden pair",       5 },
    { "hidden triple",     5 },
    { "hidden quad",       5 },
    { "naked single",      1 },
    { "naked pair",        3 },
    { "naked triple",      3 },
    { "naked quad",        3 },
    { "locked candidates", 4 },
    { "x-wing",            6 },
    { "xy-wing",           7 },
    { "swordfish",         7 },
    { "jellyfish",         8 },
    { "simple coloring",   8 }
};
static_assert((size_t)Analyzer::Step::NUMBER_OF_TECHNIQUES == sizeof(TECHNIQUE_INFO) / sizeof(*TECHNIQUE_INFO), "techniqueInfo has the wrong number of elements");

static int const HIGHEST_DIFFICULTY = 8; // from the table above

Analyzer::Analyzer(Board const & board)
    : board_(board)
    , candidates_(Board::NUM_CELLS, (Candidates::Type)Candidates::ALL)
#if defined(_DEBUG)
    , solvedBoard_(board)
#endif // defined(_DEBUG)
{
#if defined(_DEBUG)
    // Validate the board
    XCODE_COMPATIBLE_ASSERT(board_.consistent());
    XCODE_COMPATIBLE_ASSERT(Solver::hasUniqueSolution(board_));

    // Create the solved board for debugging purposes
    Solver::solve(solvedBoard_);
#endif // defined(_DEBUG)

    // Update candidates according to known cells
    Board::ForEach::cell([&] (int i) {
        if (!board.isEmpty(i))
        {
            int x = board.get(i);
            
            // The cell has only one candidate
            candidates_[i] = Candidates::fromValue(x);

            // Eliminate this cell's value from its dependents' candidates
            std::vector<int> dependents = Board::Cell::dependents(i);
            eliminate(dependents, x);
        }
        return true;
    });

#if defined(_DEBUG)
    // Sanity check -- validate the candidates
    XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
#endif // defined(_DEBUG)
}

Analyzer::Analyzer(Board const & board, Candidates::List const & candidates)
    : board_(board)
    , candidates_(candidates)
#if defined(_DEBUG)
    , solvedBoard_(board)
#endif // defined(_DEBUG)
{
#if defined(_DEBUG)
    XCODE_COMPATIBLE_ASSERT(board_.consistent());
    XCODE_COMPATIBLE_ASSERT(Solver::hasUniqueSolution(board_));

    // Create the solved board for debugging purposes
    Solver::solve(solvedBoard_);

    // Sanity check -- validate the candidates
    XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
#endif // defined(_DEBUG)
}

Analyzer::Step Analyzer::next()
{
    XCODE_COMPATIBLE_ASSERT(candidatesAreValid());

    if (board_.completed())
    {
        done_ = true;
        return { Step::DONE };
    }

    std::vector<int> indexes;
    std::vector<int> values;
    std::string reason;
    
    // Ensure that every cell has at least candidate. If not, then the board is not solvable.
    {
        for (int i = 0; i < Board::NUM_CELLS; ++i)
        {
            if (candidates_[i] == 0)
            {
                done_  = true;
                stuck_ = true;
                indexes.push_back(i);
                values.push_back(0);
                reason = "The puzzle cannot be solved. There are no candidates for " + Board::Cell::name(i) + ".";
                return { Step::STUCK, indexes, values, Step::NONE, reason  };
            }
        }
    }
    
    {
        Naked naked(board_, candidates_);
        if (naked.singleExists(indexes, values, reason))
        {
            setValue(indexes.front(), values.front());
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::SOLVE, indexes, values, Step::NAKED_SINGLE, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.singleExists(indexes, values, reason))
        {
            setValue(indexes.front(), values.front());
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::SOLVE, indexes, values, Step::HIDDEN_SINGLE, reason };
        }
    }

    {
        Naked naked(board_, candidates_);
        if (naked.pairExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::NAKED_PAIR, reason };
        }
    }

    {
        Naked naked(board_, candidates_);
        if (naked.tripleExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::NAKED_TRIPLE, reason };
        }
    }

    {
        Naked naked(board_, candidates_);
        if (naked.quadExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::NAKED_QUAD, reason };
        }
    }

    {
        LockedCandidates lockedCandidates(candidates_);
        if (lockedCandidates.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::LOCKED_CANDIDATES, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.pairExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::HIDDEN_PAIR, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.tripleExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::HIDDEN_TRIPLE, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.quadExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::HIDDEN_QUAD, reason };
        }
    }

    {
        XWing xWing(candidates_);
        if (xWing.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::X_WING, reason };
        }
    }

    {
        Swordfish swordfish(candidates_);
        if (swordfish.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::SWORDFISH, reason };
        }
    }

    {
        Jellyfish jellyfish(candidates_);
        if (jellyfish.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::JELLYFISH, reason };
        }
    }

    {
        XYWing xyWing(candidates_);
        if (xyWing.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::XY_WING, reason };
        }
    }

    {
        SimpleColoring simpleColoring(candidates_);
        if (simpleColoring.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::SIMPLE_COLORING, reason };
        }
    }

    done_  = true;
    stuck_ = true;
    return { Step::STUCK };
}

void Analyzer::drawCandidates() const
{
    printf("      1       2       3       4       5       6       7       8       9\n");
    printf("  +=======+=======+=======+=======+=======+=======+=======+=======+=======+\n");
    for (int r = 0; r < Board::SIZE; ++r)
    {
        printf("  |");
        for (int c = 0; c < Board::SIZE; ++c)
        {
            Candidates::Type candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (Candidates::isSolved(candidates) && !board_.isEmpty(r, c))
            {
                printf("       ");
            }
            else
            {
                char x1 = (candidates & Candidates::fromValue(1)) ? '1' : '.';
                char x2 = (candidates & Candidates::fromValue(2)) ? '2' : '.';
                char x3 = (candidates & Candidates::fromValue(3)) ? '3' : '.';
                printf(" %c %c %c ", x1, x2, x3);
            }
            if (c % 3 == 2)
                printf("|");
            else
                printf(":");
        }
        printf("\n");

        printf("%c |", Board::Group::rowName(r));
        for (int c = 0; c < Board::SIZE; ++c)
        {
            Candidates::Type candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (Candidates::isSolved(candidates) && !board_.isEmpty(r, c))
            {
                int v = board_.get(r, c);
                printf("   %d   ", v);
            }
            else
            {
                char x4 = (candidates & Candidates::fromValue(4)) ? '4' : '.';
                char x5 = (candidates & Candidates::fromValue(5)) ? '5' : '.';
                char x6 = (candidates & Candidates::fromValue(6)) ? '6' : '.';
                printf(" %c %c %c ", x4, x5, x6);
            }
            if (c % 3 == 2)
                printf("|");
            else
                printf(":");
        }
        printf("\n");

        printf("  |");
        for (int c = 0; c < Board::SIZE; ++c)
        {
            Candidates::Type candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (Candidates::isSolved(candidates) && !board_.isEmpty(r, c))
            {
                printf("       ");
            }
            else
            {
                char x7 = (candidates & Candidates::fromValue(7)) ? '7' : '.';
                char x8 = (candidates & Candidates::fromValue(8)) ? '8' : '.';
                char x9 = (candidates & Candidates::fromValue(9)) ? '9' : '.';
                printf(" %c %c %c ", x7, x8, x9);
            }
            if (c % 3 == 2)
                printf("|");
            else
                printf(":");
        }
        printf("\n");
        if (r % 3 == 2)
            printf("  +=======+=======+=======+=======+=======+=======+=======+=======+=======+\n");
        else
            printf("  +-------+-------+-------+-------+-------+-------+-------+-------+-------+\n");
    }
}

nlohmann::json Analyzer::toJson() const
{
    std::vector<std::vector<std::vector<int> > > candidateValues;
    candidateValues.resize(Board::SIZE);
    for (int r = 0; r < Board::SIZE; ++r)
    {
        candidateValues[r].reserve(Board::SIZE);
        for (int c = 0; c < Board::SIZE; ++c)
        {
            int i = Board::Cell::indexOf(r, c);
            Candidates::Type candidates = candidates_[i];
            std::vector<int> values     = Candidates::values(candidates);
            candidateValues[r].emplace_back(std::move(values));
        }
    }
    return json {
               { "board", board_.toJson() },
               { "candidates", candidateValues }
    };
}

// Sets cell i to x, and eliminates the value from candidates in all dependent cells
void Analyzer::setValue(int i, int x)
{
    // Update the board
    board_.set(i, x);

    // The cell has only one candidate now
    candidates_[i] = Candidates::fromValue(x);

    // Eliminate this cell's value from its dependents' candidates
    std::vector<int> dependents = Board::Cell::dependents(i);
    eliminate(dependents, x);
}

void Analyzer::eliminate(std::vector<int> const & indexes, int x)
{
    for (int i : indexes)
    {
        candidates_[i] &= ~Candidates::fromValue(x);
        XCODE_COMPATIBLE_ASSERT(candidates_[i] != 0);
    }
}

void Analyzer::eliminate(std::vector<int> const & indexes, std::vector<int> const & values)
{
    for (int v : values)
    {
        eliminate(indexes, v);
    }
}

#if defined(_DEBUG)
bool Analyzer::candidatesAreValid()
{
    return Board::ForEach::cell([&] (int i) {
        int v = solvedBoard_.get(i);
        XCODE_COMPATIBLE_ASSERT(v != Board::EMPTY); // Sanity check
        return (Candidates::fromValue(v) & candidates_[i]) != 0;
    });
}

#endif // defined(_DEBUG)

const char * Analyzer::Step::techniqueName(Analyzer::Step::TechniqueId technique)
{
    XCODE_COMPATIBLE_ASSERT((size_t)technique >= 0 && (size_t)technique < sizeof(TECHNIQUE_INFO) / sizeof(*TECHNIQUE_INFO));
    return TECHNIQUE_INFO[technique].name;
}

char const * Analyzer::Step::actionName(Analyzer::Step::ActionId action)
{
    static char const * const NAMES[] =
    {
        "solve",
        "eliminate",
        "stuck",
        "done"
    };
    XCODE_COMPATIBLE_ASSERT((size_t)action >= 0 && (size_t)action < sizeof(NAMES) / sizeof(*NAMES));
    return NAMES[action];
}

int Analyzer::Step::techniqueDifficulty(TechniqueId technique)
{
    XCODE_COMPATIBLE_ASSERT((size_t)technique >= 0 && (size_t)technique < sizeof(TECHNIQUE_INFO) / sizeof(*TECHNIQUE_INFO));
    return TECHNIQUE_INFO[technique].difficulty;
}

nlohmann::json Analyzer::Step::toJson() const
{
    json out;
    out["action"] = actionName(action);
    if (!indexes.empty())
        out["indexes"] = indexes;
    if (!values.empty())
        out["values"] = values;
    if (technique != NONE)
        out["technique"] = techniqueName(technique);
    if (!reason.empty())
        out["reason"] = reason;
    return out;
}
