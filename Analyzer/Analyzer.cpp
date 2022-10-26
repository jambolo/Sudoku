#include "Analyzer.h"

#include "Candidates.h"
#include "Hidden.h"
#include "Link.h"
#include "LockedCandidates.h"
#include "Naked.h"
#include "SimpleColoring.h"
#include "UniqueRectangle.h"
#include "XWing.h"
#include "XYWing.h"

#include "Board/Board.h"
#if defined(_DEBUG)
#include "Solver/Solver.h"
#endif // defined(_DEBUG)

#include <nlohmann/json.hpp>

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

using json = nlohmann::json;

struct TechniqueInfoEntry
{
    Analyzer::Step::TechniqueId id;
    char const * name;
    int difficulty;
    Analyzer::Step::ActionId action;
};

// Info about techniques by technique ID
static TechniqueInfoEntry const TECHNIQUE_INFO[Analyzer::Step::NUMBER_OF_TECHNIQUES] =
{
    { Analyzer::Step::NONE,              "none",              0, Analyzer::Step::STUCK     },
    { Analyzer::Step::NAKED_SINGLE,      "naked single",      1, Analyzer::Step::SOLVE     },
    { Analyzer::Step::NAKED_PAIR,        "naked pair",        2, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::NAKED_TRIPLE,      "naked triple",      3, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::NAKED_QUAD,        "naked quad",        5, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::HIDDEN_SINGLE,     "hidden single",     1, Analyzer::Step::SOLVE     },
    { Analyzer::Step::HIDDEN_PAIR,       "hidden pair",       2, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::HIDDEN_TRIPLE,     "hidden triple",     3, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::HIDDEN_QUAD,       "hidden quad",       5, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::LOCKED_CANDIDATES, "locked candidates", 4, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::X_WING,            "x-wing",            6, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::SWORDFISH,         "swordfish",         7, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::JELLYFISH,         "jellyfish",         8, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::XY_WING,           "xy-wing",           7, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::SIMPLE_COLORING,   "simple coloring",   8, Analyzer::Step::ELIMINATE },
    { Analyzer::Step::UNIQUE_RECTANGLE,  "unique rectangle",  8, Analyzer::Step::ELIMINATE }
};
static_assert((size_t)Analyzer::Step::NUMBER_OF_TECHNIQUES == sizeof(TECHNIQUE_INFO) / sizeof(*TECHNIQUE_INFO),
              "techniqueInfo has the wrong number of elements");

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
        solved_ = true;
        return { Step::DONE, Step::NONE, {}, {}, "Solved."};
    }

    // Ensure that every cell has at least one candidate. If not, then the board is not solvable.
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        if (candidates_[i] == Candidates::NONE)
        {
            std::vector<int> indexes{ i };
            std::vector<int> values{ 0 };
            std::string      reason = "The puzzle cannot be solved. There is no possible value for " + Board::Cell::name(i) + ".";
            stuck_ = true;
            return { Step::STUCK, Step::NONE, {}, {}, reason };
        }
    }

    // Technique info sorted by difficulty
    std::vector<TechniqueInfoEntry> sortedTechniqueInfo(std::begin(TECHNIQUE_INFO), std::end(TECHNIQUE_INFO));
    std::sort(sortedTechniqueInfo.begin(),
              sortedTechniqueInfo.end(),
              [](TechniqueInfoEntry const & a, TechniqueInfoEntry const & b) {
                  return a.difficulty < b.difficulty;
              });

    std::vector<int> indexes;
    std::vector<int> values;
    std::string      reason;

    // Try each technique
    for (auto const & info : sortedTechniqueInfo)
    {
        bool found = false; // True when a technique finds something

        switch (info.id)
        {
            case Step::NAKED_SINGLE:
            {
                Naked naked(board_, candidates_);
                found = naked.singleExists(indexes, values, reason);
                break;
            }
            case Step::NAKED_PAIR:
            {
                Naked naked(board_, candidates_);
                found = naked.pairExists(indexes, values, reason);
                break;
            }
            case Step::NAKED_TRIPLE:
            {
                Naked naked(board_, candidates_);
                found = naked.tripleExists(indexes, values, reason);
                break;
            }
            case Step::NAKED_QUAD:
            {
                Naked naked(board_, candidates_);
                found = naked.quadExists(indexes, values, reason);
                break;
            }
            case Step::HIDDEN_SINGLE:
            {
                Hidden hidden(board_, candidates_);
                found = hidden.singleExists(indexes, values, reason);
                break;
            }
            case Step::HIDDEN_PAIR:
            {
                Hidden hidden(board_, candidates_);
                found = hidden.pairExists(indexes, values, reason);
                break;
            }
            case Step::HIDDEN_TRIPLE:
            {
                Hidden hidden(board_, candidates_);
                found = hidden.tripleExists(indexes, values, reason);
                break;
            }
            case Step::HIDDEN_QUAD:
            {
                Hidden hidden(board_, candidates_);
                found = hidden.quadExists(indexes, values, reason);
                break;
            }
            case Step::LOCKED_CANDIDATES:
            {
                LockedCandidates lockedCandidates(candidates_);
                found = lockedCandidates.exists(indexes, values, reason);
                break;
            }
            case Step::X_WING:
            {
                XWing xWing(candidates_);
                found = xWing.exists(indexes, values, reason);
                break;
            }
            case Step::SWORDFISH:
            {
                Swordfish swordfish(candidates_);
                found = swordfish.exists(indexes, values, reason);
                break;
            }
            case Step::JELLYFISH:
            {
                Jellyfish jellyfish(candidates_);
                found = jellyfish.exists(indexes, values, reason);
                break;
            }
            case Step::XY_WING:
            {
                XYWing xyWing(candidates_);
                found = xyWing.exists(indexes, values, reason);
                break;
            }
            case Step::SIMPLE_COLORING:
            {
                SimpleColoring simpleColoring(candidates_);
                found = simpleColoring.exists(indexes, values, reason);
                break;
            }
            case Step::UNIQUE_RECTANGLE:
            {
                UniqueRectangle uniqueRectangle(candidates_);
                found = uniqueRectangle.exists(indexes, values, reason);
                break;
            }
        }

        if (found)
        {
            switch (info.action)
            {
                case Step::SOLVE:
                    setValue(indexes.front(), values.front());
                    XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
                    break;
                case Step::ELIMINATE:
                    eliminate(indexes, values);
                    XCODE_COMPATIBLE_ASSERT(candidatesAreValid());
                    break;
                case Step::STUCK:
                    break;
                case Step::DONE:
                    // Should never get here
                    XCODE_COMPATIBLE_ASSERT(info.action != Step::DONE);
                    break;
            }
            return { info.action, info.id, indexes, values, reason };
        }
    }

    // If we get here, then none of the techniques found anything
    stuck_ = true;
    return { Step::STUCK, Step::NONE, {}, {}, "I'm stuck." };
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
    std::vector<std::vector<std::vector<int>>> candidateValues;
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
    return json{
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
    for (auto i : indexes)
    {
        candidates_[i] &= ~Candidates::fromValue(x);
        XCODE_COMPATIBLE_ASSERT(candidates_[i] != 0);
    }
}

void Analyzer::eliminate(std::vector<int> const & indexes, std::vector<int> const & values)
{
    for (auto v : values)
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
    XCODE_COMPATIBLE_ASSERT(technique >= 0 && (size_t)technique < sizeof(TECHNIQUE_INFO) / sizeof(*TECHNIQUE_INFO));
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
    XCODE_COMPATIBLE_ASSERT(technique >= 0 && (size_t)technique < sizeof(TECHNIQUE_INFO) / sizeof(*TECHNIQUE_INFO));
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
