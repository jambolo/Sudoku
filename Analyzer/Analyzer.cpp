#include "Analyzer.h"

#include "Candidates.h"
#include "Hidden.h"
#include "Link.h"
#include "LockedCandidates.h"
#include "Naked.h"
#include "XWing.h"
#include "YWing.h"

#include "Board/Board.h"
#if defined(_DEBUG)
#include "Solver/Solver.h"
#endif // defined(_DEBUG)

#include "SimpleColoring.h"
#include <algorithm>
#include <cassert>
#include <numeric>

Analyzer::Analyzer(Board const & board)
    : board_(board)
    , candidates_(Board::SIZE * Board::SIZE, Candidates::ALL)
#if defined(_DEBUG)
    , solvedBoard_(board)
#endif // defined(_DEBUG)
{
#if defined(_DEBUG)
    // Validate the board
    assert(board_.consistent());
    assert(Solver::hasUniqueSolution(board_));

    // Create the solved board for debugging purposes
    Solver::solve(solvedBoard_);
#endif // defined(_DEBUG)

    // Update candidates according to known cells
    Board::ForEach::cell([&] (int i) {
        if (!board.isEmpty(i))
        {
            int x = board.get(i);
            solve(i, x);
        }
        return true;
    });

#if defined(_DEBUG)
    // Sanity check -- validate the candidates
    assert(candidatesAreValid());
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
    assert(board_.consistent());
    assert(Solver::hasUniqueSolution(board_));

    // Create the solved board for debugging purposes
    Solver::solve(solvedBoard_);

    // Sanity check -- validate the candidates
    assert(candidatesAreValid());
#endif // defined(_DEBUG)
}

Analyzer::Step Analyzer::next()
{
    assert(candidatesAreValid());

    if (board_.completed())
    {
        done_ = true;
        return { Step::DONE };
    }

    std::vector<int> indexes;
    std::vector<int> values;
    std::string reason;

    {
        Naked naked(board_, candidates_);
        if (naked.singleExists(indexes, values, reason))
        {
            solve(indexes.front(), values.front());
            assert(candidatesAreValid());
            return { Step::SOLVE, indexes, values, Step::NAKED_SINGLE, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.singleExists(indexes, values, reason))
        {
            solve(indexes.front(), values.front());
            assert(candidatesAreValid());
            return { Step::SOLVE, indexes, values, Step::HIDDEN_SINGLE, reason };
        }
    }

    {
        Naked naked(board_, candidates_);
        if (naked.pairExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::NAKED_PAIR, reason };
        }
    }

    {
        Naked naked(board_, candidates_);
        if (naked.tripleExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::NAKED_TRIPLE, reason };
        }
    }

    {
        Naked naked(board_, candidates_);
        if (naked.quadExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::NAKED_QUAD, reason };
        }
    }

    {
        LockedCandidates lockedCandidates(board_, candidates_);
        if (lockedCandidates.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::LOCKED_CANDIDATES, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.pairExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::HIDDEN_PAIR, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.tripleExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::HIDDEN_TRIPLE, reason };
        }
    }

    {
        Hidden hidden(board_, candidates_);
        if (hidden.quadExists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::HIDDEN_QUAD, reason };
        }
    }

    {
        XWing xWing(candidates_);
        if (xWing.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::X_WING, reason };
        }
    }

    {
        YWing yWing(candidates_);
        if (yWing.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
            return { Step::ELIMINATE, indexes, values, Step::Y_WING, reason };
        }
    }

    {
        SimpleColoring simpleColoring(candidates_);
        if (simpleColoring.exists(indexes, values, reason))
        {
            eliminate(indexes, values);
            assert(candidatesAreValid());
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
            if (Candidates::solved(candidates) && !board_.isEmpty(r, c))
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

        printf("%c |", Board::Unit::rowName(r));
        for (int c = 0; c < Board::SIZE; ++c)
        {
            Candidates::Type candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (Candidates::solved(candidates) && !board_.isEmpty(r, c))
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
            if (Candidates::solved(candidates) && !board_.isEmpty(r, c))
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

void Analyzer::solve(int i, int x)
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
        assert(candidates_[i] != 0);
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
        assert(v != Board::EMPTY); // Sanity check
        return (Candidates::fromValue(v) & candidates_[i]) != 0;
    });
}

#endif // defined(_DEBUG)

const char * Analyzer::Step::techniqueName(Analyzer::Step::TechniqueId technique)
{
    static char const * const NAMES[] =
    {
        "none",
        "hidden single",
        "hidden pair",
        "hidden triple",
        "hidden quad",
        "naked single",
        "naked pair",
        "naked triple",
        "naked quad",
        "locked candidates",
        "x-wing",
        "y-wing",
        "simple coloring"
    };
    assert((size_t)technique >= 0 && (size_t)technique < sizeof(NAMES) / sizeof(*NAMES));
    return NAMES[technique];
}
