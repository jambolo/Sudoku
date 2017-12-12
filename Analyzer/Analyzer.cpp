#include "Analyzer.h"

#include "Board/Board.h"
#if defined(_DEBUG)
#include "Solver/Solver.h"
#endif // defined(_DEBUG)

#include <algorithm>
#include <cassert>
#include <numeric>

static int const ALL_CANDIDATES = 0x3fe;

// Calls a function for element of a vector except the specified ones
static void for_each_index_except(std::vector<int> const & indexes, int x0, std::function<void(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0)
            f(i);
    }
}

// Calls a function for element of a vector except the specified ones
static void for_each_index_except(std::vector<int> const & indexes, int x0, int x1, std::function<void(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0 && i != x1)
            f(i);
    }
}

// Calls a function for element of a vector except the specified ones
static void for_each_index_except(std::vector<int> const & indexes,
                                  int                      x0,
                                  int                      x1,
                                  int                      x2,
                                  std::function<void(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0 && i != x1 && i != x2)
            f(i);
    }
}

// Calls a function for element of a vector except the specified ones
static void for_each_index_except(std::vector<int> const & indexes,
                                  int                      x0,
                                  int                      x1,
                                  int                      x2,
                                  int                      x3,
                                  std::function<void(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0 && i != x1 && i != x2 && i != x3)
            f(i);
    }
}

// Returns true if there is only one candidate
static bool solved(unsigned candidates)
{
    return (candidates & (candidates - 1)) == 0;
}

// Returns the value of the only candidate
static int valueFromCandidates(unsigned candidates)
{
    int v = 0;
    while (candidates)
    {
        candidates >>= 1;
        ++v;
        if (candidates & 1)
            return v;
    }
    return 0;
}

// Returns a list of of candidates
static std::vector<int> valuesFromCandidates(unsigned candidates)
{
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

static int candidateCount(unsigned candidates)
{
    int count = 0;
    while (candidates != 0)
    {
        candidates >>= 1;
        if (candidates & 1)
            ++count;
    }
    return count;
}

static unsigned candidate(int v)
{
    return 1 << v;
}

Analyzer::Analyzer(Board const & board, bool verbose /*= false*/)
    : board_(board)
    , verbose_(verbose)
    , candidates_(Board::SIZE * Board::SIZE, ALL_CANDIDATES)
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

Analyzer::Analyzer(Board const & board, std::vector<unsigned> const & candidates, bool verbose /*= false*/)
    : board_(board)
    , verbose_(verbose)
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

    if (nakedSingleFound(indexes, values, reason))
    {
        solve(indexes.front(), values.front());
        assert(candidatesAreValid());
        return { Step::SOLVE, indexes, values, Step::NAKED_SINGLE, reason };
    }

    if (hiddenSingleFound(indexes, values, reason))
    {
        solve(indexes.front(), values.front());
        assert(candidatesAreValid());
        return { Step::SOLVE, indexes, values, Step::HIDDEN_SINGLE, reason };
    }

    if (nakedPairFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::NAKED_PAIR, reason };
    }

    if (nakedTripleFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::NAKED_TRIPLE, reason };
    }

    if (nakedQuadFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::NAKED_QUAD, reason };
    }

    if (lockedCandidatesFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::LOCKED_CANDIDATES, reason };
    }

    if (hiddenPairFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_PAIR, reason };
    }

    if (hiddenTripleFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_TRIPLE, reason };
    }

    if (hiddenQuadFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_QUAD, reason };
    }

    if (xWingFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::X_WING, reason };
    }

    if (yWingFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        assert(candidatesAreValid());
        return { Step::ELIMINATE, indexes, values, Step::Y_WING, reason };
    }

    done_  = true;
    stuck_ = true;
    return { Step::STUCK };
}

void Analyzer::drawPenciledBoard() const
{
    printf("      1       2       3       4       5       6       7       8       9\n");
    printf("  +=======+=======+=======+=======+=======+=======+=======+=======+=======+\n");
    for (int r = 0; r < Board::SIZE; ++r)
    {
        printf("  |");
        for (int c = 0; c < Board::SIZE; ++c)
        {
            unsigned candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (solved(candidates) && !board_.isEmpty(r, c))
            {
                printf("       ");
            }
            else
            {
                char x1 = (candidates & candidate(1)) ? '1' : '.';
                char x2 = (candidates & candidate(2)) ? '2' : '.';
                char x3 = (candidates & candidate(3)) ? '3' : '.';
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
            unsigned candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (solved(candidates) && !board_.isEmpty(r, c))
            {
                int v = board_.get(r, c);
                printf("   %d   ", v);
            }
            else
            {
                char x4 = (candidates & candidate(4)) ? '4' : '.';
                char x5 = (candidates & candidate(5)) ? '5' : '.';
                char x6 = (candidates & candidate(6)) ? '6' : '.';
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
            unsigned candidates = candidates_[Board::Cell::indexOf(r, c)];
            if (solved(candidates) && !board_.isEmpty(r, c))
            {
                printf("       ");
            }
            else
            {
                char x7 = (candidates & candidate(7)) ? '7' : '.';
                char x8 = (candidates & candidate(8)) ? '8' : '.';
                char x9 = (candidates & candidate(9)) ? '9' : '.';
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
    candidates_[i] = candidate(x);

    // Eliminate this cell's value from its dependents' candidates
    std::vector<int> dependents = Board::Cell::dependents(i);
    eliminate(dependents, x);
}

void Analyzer::eliminate(std::vector<int> const & indexes, int x)
{
    for (int i : indexes)
    {
        candidates_[i] &= ~candidate(x);
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

bool Analyzer::hiddenSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    bool found;
    int which;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (hiddenSingle(row, indexes, values))
        {
            which = r;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = std::string("This is the only square in row ") + Board::Unit::rowName(which) + " that can have this value.";
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (hiddenSingle(column, indexes, values))
        {
            which = c;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = std::string("This is the only square in column ") + Board::Unit::columnName(which) +
                     " that can have this value.";
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (hiddenSingle(box, indexes, values))
        {
            which = b;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = std::string("This is the only square in box ") + Board::Unit::boxName(which) + " that can have this value.";
        return true;
    }

    return false;
}

bool Analyzer::hiddenSingle(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues) const
{
    for (int s : indexes)
    {
        if (board_.isEmpty(s))
        {
            unsigned others = 0;
            for_each_index_except(indexes, s, [&] (int i) {
                others |= candidates_[i];
            });

            unsigned exclusive = candidates_[s] & ~others;
            if (exclusive)
            {
                assert(solved(exclusive));
                eliminatedIndexes.push_back(s);
                eliminatedValues.push_back(valueFromCandidates(exclusive));
                return true;
            }
        }
    }
    return false;
}

static std::string generateHiddenPairReason(std::string const & unitType, char which, std::vector<int> const & hiddenValues)
{
    std::string reason = "Only these two squares in " + unitType + " " + which +
                         " can be " + std::to_string(hiddenValues[0]) +
                         " or " + std::to_string(hiddenValues[1]) +
                         ", so they cannot be any other values.";
    return reason;
}

bool Analyzer::hiddenPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each exclusive pair in a unit, if they have additional candidates, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (hiddenPair(row, indexes, values, hidden))
        {
            which = r;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenPairReason("row", Board::Unit::rowName(which), hidden);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (hiddenPair(column, indexes, values, hidden))
        {
            which = c;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenPairReason("column", Board::Unit::columnName(which), hidden);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (hiddenPair(box, indexes, values, hidden))
        {
            which = b;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenPairReason("box", Board::Unit::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Analyzer::hiddenPair(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues,
                          std::vector<int> &       hiddenValues) const
{
    // Go through each possible pair of candidates and search for exactly two cells containing the one or more of a pair
    for (int x0 = 1; x0 <= Board::SIZE - 1; ++x0)
    {
        unsigned m0 = candidate(x0);
        for (int x1 = x0 + 1; x1 <= Board::SIZE; ++x1)
        {
            unsigned m1 = candidate(x1);
            unsigned m  = m0 | m1;

            // Count the number of cells with either of these two candidates
            int nFound    = 0;
            int found[2];
            int counts[2] = { 0 };
            for (int i : indexes)
            {
                unsigned candidates = candidates_[i];
                if (candidates & m)
                {
                    // Count the number of found indexes
                    ++nFound;
                    if (nFound > 2)
                        break;  // Give up, must be exactly 2
                    found[nFound - 1] = i;

                    // Count the number of times the candidates occur
                    if (candidates & m0)
                        ++counts[0];
                    if (candidates & m1)
                        ++counts[1];
                }
            }
            // If there are exactly 2 indexes, each candidate is seen at least twice, and there are other candidates to eliminate,
            // then success
            if (nFound == 2 && counts[0] >= 2 && counts[1] >= 2)
            {
                unsigned eliminatedCandidates = (candidates_[found[0]] | candidates_[found[1]]) & ~m;
                if (eliminatedCandidates)
                {
                    eliminatedIndexes.assign(std::begin(found), std::end(found));
                    eliminatedValues = valuesFromCandidates(eliminatedCandidates);
                    hiddenValues     = valuesFromCandidates(m);
                    return true;
                }
            }
        }
    }

    return false;
}

static std::string generateHiddenTripleReason(std::string const & unitType, char which, std::vector<int> const & hiddenValues)
{
    std::string reason = "Only these three squares in " + unitType + " " + which +
                         " can be " + std::to_string(hiddenValues[0]) +
                         ", " + std::to_string(hiddenValues[1]) +
                         ", or " + std::to_string(hiddenValues[2]) +
                         ", so they cannot be any other values.";
    return reason;
}

bool Analyzer::hiddenTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each exclusive triple in a unit, if they have additional candidates, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (hiddenTriple(row, indexes, values, hidden))
        {
            which = r;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenTripleReason("row", Board::Unit::rowName(which), hidden);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (hiddenTriple(column, indexes, values, hidden))
        {
            which = c;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenTripleReason("column", Board::Unit::columnName(which), hidden);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (hiddenTriple(box, indexes, values, hidden))
        {
            which = b;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenTripleReason("box", Board::Unit::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Analyzer::hiddenTriple(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues,
                            std::vector<int> &       hiddenValues) const
{
    // Go through each possible triple of candidates and search for exactly three cells containing the one or more of a triple
    for (int x0 = 1; x0 <= Board::SIZE - 2; ++x0)
    {
        unsigned m0 = candidate(x0);
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 1; ++x1)
        {
            unsigned m1 = candidate(x1);
            for (int x2 = x1 + 1; x2 <= Board::SIZE; ++x2)
            {
                unsigned m2 = candidate(x2);
                unsigned m  = m0 | m1 | m2;

                // Count the number of cells with any of these three candidates
                int nFound    = 0;
                int found[3];
                int counts[3] = { 0 };
                for (int i : indexes)
                {
                    unsigned candidates = candidates_[i];
                    if (candidates & m)
                    {
                        // Count the number of found indexes
                        ++nFound;
                        if (nFound > 3)
                            break;  // Give up, must be exactly 3
                        found[nFound - 1] = i;

                        // Count the number of times the candidates occur
                        if (candidates & m0)
                            ++counts[0];
                        if (candidates & m1)
                            ++counts[1];
                        if (candidates & m2)
                            ++counts[2];
                    }
                }
                // If there are exactly 3 indexes, each candidate is seen at least twice, and there are other candidates to
                // eliminate,
                // then success
                if (nFound == 3 && counts[0] >= 2 && counts[1] >= 2 && counts[2] >= 2)
                {
                    unsigned eliminatedCandidates = (candidates_[found[0]] | candidates_[found[1]] | candidates_[found[2]]) & ~m;
                    if (eliminatedCandidates)
                    {
                        eliminatedIndexes.assign(std::begin(found), std::end(found));
                        eliminatedValues = valuesFromCandidates(eliminatedCandidates);
                        hiddenValues     = valuesFromCandidates(m);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static std::string generateHiddenQuadReason(std::string const & unitType, char which, std::vector<int> const & hiddenValues)
{
    std::string reason = "Only these four squares in " + unitType + " " + which +
                         " can be " + std::to_string(hiddenValues[0]) +
                         ", " + std::to_string(hiddenValues[1]) +
                         ", " + std::to_string(hiddenValues[2]) +
                         ", or " + std::to_string(hiddenValues[3]) +
                         ", so they cannot be any other values.";
    return reason;
}

bool Analyzer::hiddenQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each exclusive quad in a unit, if they have additional candidates, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (hiddenQuad(row, indexes, values, hidden))
        {
            which = r;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenQuadReason("row", Board::Unit::rowName(which), hidden);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (hiddenQuad(column, indexes, values, hidden))
        {
            which = c;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenQuadReason("column", Board::Unit::columnName(which), hidden);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (hiddenQuad(box, indexes, values, hidden))
        {
            which = b;
            return false;   // done
        }
        else
        {
            return true;
        }
    });
    if (found)
    {
        if (verbose_)
            reason = generateHiddenQuadReason("box", Board::Unit::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Analyzer::hiddenQuad(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues,
                          std::vector<int> &       hiddenValues) const
{
    // Go through each possible quad of candidates and search for exactly four cells containing the one or more of a quad
    for (int x0 = 1; x0 <= Board::SIZE - 3; ++x0)
    {
        unsigned m0 = candidate(x0);
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 2; ++x1)
        {
            unsigned m1 = candidate(x1);
            for (int x2 = x1 + 1; x2 <= Board::SIZE - 1; ++x2)
            {
                unsigned m2 = candidate(x2);
                for (int x3 = x2 + 1; x3 <= Board::SIZE; ++x3)
                {
                    unsigned m3 = candidate(x3);
                    unsigned m  = m0 | m1 | m2 | m3;

                    // Count the number of cells with any of these four candidates
                    int nFound    = 0;
                    int found[4];
                    int counts[4] = { 0 };
                    for (int i : indexes)
                    {
                        unsigned candidates = candidates_[i];
                        if (candidates & m)
                        {
                            // Count the number of found indexes
                            ++nFound;
                            if (nFound > 4)
                                break;  // Give up, must be exactly 4
                            found[nFound - 1] = i;

                            // Count the number of times the candidates occur
                            if (candidates & m0)
                                ++counts[0];
                            if (candidates & m1)
                                ++counts[1];
                            if (candidates & m2)
                                ++counts[2];
                            if (candidates & m3)
                                ++counts[3];
                        }
                    }
                    // If there are exactly 4 indexes, each candidate is seen at least twice, and there are other candidates to
                    // eliminate,
                    // then success
                    if (nFound == 4 && counts[0] >= 2 && counts[1] >= 2 && counts[2] >= 2 && counts[3] >= 2)
                    {
                        unsigned eliminatedCandidates =
                            (candidates_[found[0]] | candidates_[found[1]] | candidates_[found[2]] | candidates_[found[3]]) & ~m;
                        if (eliminatedCandidates)
                        {
                            eliminatedIndexes.assign(std::begin(found), std::end(found));
                            eliminatedValues = valuesFromCandidates(eliminatedCandidates);
                            hiddenValues     = valuesFromCandidates(m);
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Analyzer::nakedSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each unsolved cell, if it only has one candidate, then success

    if (nakedSingle(indexes, values))
    {
        if (verbose_)
            reason = "There are no other possible values for this square.";
        return true;
    }
    return false;
}

bool Analyzer::nakedSingle(std::vector<int> & indexes, std::vector<int> & values) const
{
    return !Board::ForEach::cell([&] (int i) {
        if (board_.isEmpty(i))
        {
            unsigned candidates = candidates_[i];
            if (solved(candidates))
            {
                indexes.push_back(i);
                values.push_back(valueFromCandidates(candidates));
                return false;
            }
        }
        return true;
    });
}

static std::string generateNakedPairReason(std::string const & unitType, char which, std::vector<int> const & nakedIndexes)
{
    std::string reason = "Two other squares (" + Board::Cell::name(nakedIndexes[0]) +
                         " and " + Board::Cell::name(nakedIndexes[1]) +
                         ") in " + unitType + " " + which +
                         " must be one of these two values, so these squares cannot be either of these two values.";
    return reason;
}

bool Analyzer::nakedPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each exclusive pair in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (nakedPair(row, indexes, values, nakedIndexes))
        {
            which = r;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedPairReason("row", Board::Unit::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (nakedPair(column, indexes, values, nakedIndexes))
        {
            which = c;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedPairReason("column", Board::Unit::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (nakedPair(box, indexes, values, nakedIndexes))
        {
            which = b;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedPairReason("box", Board::Unit::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Analyzer::nakedPair(std::vector<int> const & indexes,
                         std::vector<int> &       eliminatedIndexes,
                         std::vector<int> &       eliminatedValues,
                         std::vector<int> &       nakedIndexes) const
{
    for (int b0 = 0; b0 < Board::SIZE - 1; ++b0)
    {
        int i0 = indexes[b0];
        unsigned candidates0 = candidates_[i0];
        unsigned cumulativeCandidates0 = candidates0;
        int count0 = candidateCount(candidates0);
        if (count0 > 1 && count0 <= 2)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE; ++b1)
            {
                int i1 = indexes[b1];
                unsigned candidates = candidates_[i1];
                unsigned cumulativeCandidates = cumulativeCandidates0 | candidates;
                if (candidateCount(candidates) > 1 && candidateCount(cumulativeCandidates) == 2)
                {
                    for_each_index_except(indexes, i0, i1, [&] (int i) {
                        if (candidates_[i] & cumulativeCandidates)
                            eliminatedIndexes.push_back(i);
                    });
                    if (!eliminatedIndexes.empty())
                    {
                        eliminatedValues = valuesFromCandidates(cumulativeCandidates);
                        nakedIndexes     = { i0, i1 };
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

static std::string generateNakedTripleReason(std::string const & unitType, char which, std::vector<int> const & nakedIndexes)
{
    std::string reason = std::string("Three other squares (") + Board::Cell::name(nakedIndexes[0]) +
                         ", " + Board::Cell::name(nakedIndexes[1]) +
                         " and " + Board::Cell::name(nakedIndexes[2]) +
                         ") in " + unitType + " " + which +
                         " must be one of these three values, so these squares cannot be any of these three values.";
    return reason;
}

bool Analyzer::nakedTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each exclusive triple in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (nakedTriple(row, indexes, values, nakedIndexes))
        {
            which = r;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedTripleReason("row", Board::Unit::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (nakedTriple(column, indexes, values, nakedIndexes))
        {
            which = c;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedTripleReason("column", Board::Unit::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (nakedTriple(box, indexes, values, nakedIndexes))
        {
            which = b;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedTripleReason("box", Board::Unit::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Analyzer::nakedTriple(std::vector<int> const & indexes,
                           std::vector<int> &       eliminatedIndexes,
                           std::vector<int> &       eliminatedValues,
                           std::vector<int> &       nakedIndexes) const
{
    for (int b0 = 0; b0 < Board::SIZE - 2; ++b0)
    {
        int i0 = indexes[b0];
        unsigned candidates0 = candidates_[i0];
        unsigned cumulativeCandidates0 = candidates0;
        int count0 = candidateCount(candidates0);
        if (count0 > 1 && count0 <= 3)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 1; ++b1)
            {
                int i1 = indexes[b1];
                unsigned candidates1 = candidates_[i1];
                unsigned cumulativeCandidates1 = cumulativeCandidates0 | candidates1;
                if (candidateCount(candidates1) > 1 && candidateCount(cumulativeCandidates1) <= 3)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE; ++b2)
                    {
                        int i2 = indexes[b2];
                        unsigned candidates = candidates_[i2];
                        unsigned cumulativeCandidates = cumulativeCandidates1 | candidates;
                        if (candidateCount(candidates) > 1 && candidateCount(cumulativeCandidates) == 3)
                        {
                            for_each_index_except(indexes, i0, i1, i2, [&] (int i) {
                                if (candidates_[i] & cumulativeCandidates)
                                    eliminatedIndexes.push_back(i);
                            });
                            if (!eliminatedIndexes.empty())
                            {
                                eliminatedValues = valuesFromCandidates(cumulativeCandidates);
                                nakedIndexes     = { i0, i1, i2 };
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

static std::string generateNakedQuadReason(std::string const & unitType, char which, std::vector<int> const & nakedIndexes)
{
    std::string reason = "Four other squares (" + Board::Cell::name(nakedIndexes[0]) +
                         ", " + Board::Cell::name(nakedIndexes[1]) +
                         ", " + Board::Cell::name(nakedIndexes[2]) +
                         " and " + Board::Cell::name(nakedIndexes[3]) +
                         ") in " + unitType + " " + which +
                         " must be one of these four values, so these squares cannot be any of these four values.";
    return reason;
}

bool Analyzer::nakedQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For each exclusive quad in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !Board::ForEach::row([&] (int r, std::vector<int> const & row) {
        if (nakedQuad(row, indexes, values, nakedIndexes))
        {
            which = r;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedQuadReason("row", Board::Unit::rowName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::column([&] (int c, std::vector<int> const & column) {
        if (nakedQuad(column, indexes, values, nakedIndexes))
        {
            which = c;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedQuadReason("column", Board::Unit::columnName(which), nakedIndexes);
        return true;
    }

    found = !Board::ForEach::box([&] (int b, std::vector<int> const & box) {
        if (nakedQuad(box, indexes, values, nakedIndexes))
        {
            which = b;
            return false;   // done
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateNakedQuadReason("box", Board::Unit::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Analyzer::nakedQuad(std::vector<int> const & indexes,
                         std::vector<int> &       eliminatedIndexes,
                         std::vector<int> &       eliminatedValues,
                         std::vector<int> &       nakedIndexes) const
{
    for (int b0 = 0; b0 < Board::SIZE - 3; ++b0)
    {
        int i0 = indexes[b0];
        unsigned candidates0 = candidates_[i0];
        unsigned cumulativeCandidates0 = candidates0;
        int count0 = candidateCount(candidates0);
        if (count0 > 1 && count0 <= 4)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 2; ++b1)
            {
                int i1 = indexes[b1];
                unsigned candidates1 = candidates_[i1];
                unsigned cumulativeCandidates1 = cumulativeCandidates0 | candidates1;
                if (candidateCount(candidates1) > 1 && candidateCount(cumulativeCandidates1) <= 4)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE - 1; ++b2)
                    {
                        int i2 = indexes[b2];
                        unsigned candidates2 = candidates_[i2];
                        unsigned cumulativeCandidates2 = cumulativeCandidates1 | candidates2;
                        if (candidateCount(candidates2) > 1 && candidateCount(cumulativeCandidates2) <= 4)
                        {
                            for (int b3 = b2 + 1; b3 < Board::SIZE; ++b3)
                            {
                                int i3 = indexes[b3];
                                unsigned candidates = candidates_[i3];
                                unsigned cumulativeCandidates = cumulativeCandidates2 | candidates;
                                if (candidateCount(candidates) > 1 && candidateCount(cumulativeCandidates) == 4)
                                {
                                    for_each_index_except(indexes, i0, i1, i2, i3, [&] (int i) {
                                        if (candidates_[i] & cumulativeCandidates)
                                            eliminatedIndexes.push_back(i);
                                    });
                                    if (!eliminatedIndexes.empty())
                                    {
                                        eliminatedValues = valuesFromCandidates(cumulativeCandidates);
                                        nakedIndexes     = { i0, i1, i2, i3 };
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

static std::string generateLockedCandidatesReason(std::string const & unitType1,
                                                  char                which1,
                                                  std::string const & unitType2,
                                                  char                which2)
{
    std::string reason = "Since the portion of " + unitType2 + " " + which2 +
                         " within " + unitType1 + " " + which1 +
                         " must contain these values, they cannot be anywhere else in " + unitType2 + " " + which2;
    return reason;
}

bool Analyzer::lockedCandidatesFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the row/column, then success if those candidates exist in the box.

    bool found;
    int which1, which2;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        for (int c = 0; c < Board::SIZE; c += Board::BOX_SIZE)
        {
            int b = Board::Unit::whichBox(r, c);
            std::vector<int> box = Board::Unit::box(b);
            if (lockedCandidates(row, box, indexes, values))
            {
                which1 = r;
                which2 = b;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        reason = generateLockedCandidatesReason("row", Board::Unit::rowName(which1), "box", Board::Unit::boxName(which2));
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        for (int r = 0; r < Board::SIZE; r += Board::BOX_SIZE)
        {
            int b = Board::Unit::whichBox(r, c);
            std::vector<int> box = Board::Unit::box(b);
            if (lockedCandidates(column, box, indexes, values))
            {
                which1 = c;
                which2 = b;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateLockedCandidatesReason("column", Board::Unit::columnName(which1), "box", Board::Unit::boxName(which2));
        return true;
    }

    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the box, then success if those candidates exist in the row/column.

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        for (int c = 0; c < Board::SIZE; c += Board::BOX_SIZE)
        {
            int b = Board::Unit::whichBox(r, c);
            std::vector<int> box = Board::Unit::box(b);
            if (lockedCandidates(box, row, indexes, values))
            {
                which1 = b;
                which2 = r;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateLockedCandidatesReason("box", Board::Unit::boxName(which1), "row", Board::Unit::rowName(which2));
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        for (int r = 0; r < Board::SIZE; r += Board::BOX_SIZE)
        {
            int b = Board::Unit::whichBox(r, c);
            std::vector<int> box = Board::Unit::box(b);
            if (lockedCandidates(box, column, indexes, values))
            {
                which1 = b;
                which2 = c;
                return false;   // done
            }
        }
        return true;
    });
    if (found)
    {
        if (verbose_)
            reason = generateLockedCandidatesReason("box", Board::Unit::boxName(which1), "row", Board::Unit::columnName(which2));
        return true;
    }

    return false;
}

bool Analyzer::lockedCandidates(std::vector<int> const & indexes1,
                                std::vector<int> const & indexes2,
                                std::vector<int> &       eliminatedIndexes,
                                std::vector<int> &       eliminatedValues) const
{
    // Indexes in the intersection
    std::vector<int> intersection;
    std::set_intersection(indexes1.begin(), indexes1.end(),
                          indexes2.begin(), indexes2.end(),
                          std::back_inserter(intersection));

    // Indexes not in the intersection
    std::vector<int> others1;
    std::set_difference(indexes1.begin(), indexes1.end(),
                        intersection.begin(), intersection.end(),
                        std::back_inserter(others1));
    std::vector<int> others2;
    std::set_difference(indexes2.begin(), indexes2.end(),
                        intersection.begin(), intersection.end(),
                        std::back_inserter(others2));

    // Candidates in the intersection
    unsigned intersectionCandidates = allCandidates(intersection);

    // Candidates in set 1, not in intersection
    unsigned otherCandidates1 = allCandidates(others1);

    // If any of the candidates in the intersection don't exist in the rest of set 1, then they are the ones to be eliminated from
    // set 2
    unsigned unique1 = intersectionCandidates & ~otherCandidates1;
    if (unique1)
    {
        for (int i : others2)
        {
            if (candidates_[i] & unique1)
                eliminatedIndexes.push_back(i);
        }
        if (!eliminatedIndexes.empty())
        {
            std::vector<int> uniqueCandidates = valuesFromCandidates(unique1);
            eliminatedValues.insert(eliminatedValues.end(), uniqueCandidates.begin(), uniqueCandidates.end());
        }

        std::sort(eliminatedIndexes.begin(), eliminatedIndexes.end());
        eliminatedIndexes.erase(std::unique(eliminatedIndexes.begin(), eliminatedIndexes.end()), eliminatedIndexes.end());
        std::sort(eliminatedValues.begin(), eliminatedValues.end());
        eliminatedValues.erase(std::unique(eliminatedValues.begin(), eliminatedValues.end()), eliminatedValues.end());
    }

    return !eliminatedIndexes.empty();
}

static std::string generateXWingRowReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 4);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r3, c3;
    Board::Cell::locationOf(pivots[3], &r3, &c3);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) +
                         " and " + Board::Cell::name(pivots[1]) +
                         " in row " + Board::Unit::rowName(r0) +
                         " and only " + Board::Cell::name(pivots[2]) +
                         " and " + Board::Cell::name(pivots[3]) +
                         " in row " + Board::Unit::rowName(r3) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same two columns, " + Board::Unit::columnName(c0) +
                         " and " + Board::Unit::columnName(c3) +
                         ", so one of the squares in each column must have this value and none of the other squares"
                         " in these columns can.";
    return reason;
}

static std::string generateXWingColumnReason(int value, std::vector<int> const & pivots)
{
    assert(pivots.size() == 4);
    int r0, c0;
    Board::Cell::locationOf(pivots[0], &r0, &c0);
    int r3, c3;
    Board::Cell::locationOf(pivots[3], &r3, &c3);

    std::string reason = "Only " + Board::Cell::name(pivots[0]) +
                         " and " + Board::Cell::name(pivots[1]) +
                         " in column " + Board::Unit::columnName(c0) +
                         " and only " + Board::Cell::name(pivots[2]) +
                         " and " + Board::Cell::name(pivots[3]) +
                         " in column " + Board::Unit::columnName(c3) +
                         " can have the value " + std::to_string(value) +
                         ". These squares are in the same two rows, " + Board::Unit::rowName(r0) +
                         " and " + Board::Unit::rowName(r3) +
                         ". One of the squares in each row must have this value and so none of the other squares in"
                         " these rows can.";
    return reason;
}

bool Analyzer::xWingFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // If a value is a candidate in a row (or column) in exactly two cells and it is a candidate in another row
    // (or column) in exactly the same two columns (or rows) as the first row (or or column), then it cannot be
    // a candidate in any other cells in those two columns (or rows).

    bool found;
    std::vector<int> pivots;

    found = !Board::ForEach::row([&](int r, std::vector<int> const & row) {
        return !xWingRow(r, row, indexes, values, pivots);
    });
    if (found)
    {
        reason = generateXWingRowReason(values[0], pivots);
        return true;
    }

    found = !Board::ForEach::column([&](int c, std::vector<int> const & column) {
        return !xWingColumn(c, column, indexes, values, pivots);
    });
    if (found)
    {
        reason = generateXWingColumnReason(values[0], pivots);
        return true;
    }

    return false;
}

bool Analyzer::xWingRow(int                      r0,
                        std::vector<int> const & row,
                        std::vector<int> &       eliminatedIndexes,
                        std::vector<int> &       eliminatedValues,
                        std::vector<int> &       pivots) const
{
    StrongList links = findStrongLinks(row);
    for (auto link : links)
    {
        int c0        = link.u0;
        int c1        = link.u1;
        int v         = link.value;
        unsigned mask = candidate(v);

        for (int r1 = r0 + 1; r1 < Board::SIZE; ++r1)
        {
            std::vector<int> otherRow = Board::Unit::row(r1);
            int other0 = otherRow[c0];
            int other1 = otherRow[c1];

            // If there is a strong link with the same value in the same columns in another row, then
            // this is an x-wing.
            if (hasStrongLink(other0, other1, mask, otherRow))
            {
                // Look for other cells in these two columns with the value as a candidate
                for (int c : { c0, c1 })
                {
                    std::vector<int> column = Board::Unit::column(c);
                    for_each_index_except(column, column[r0], column[r1], [&](int i) {
                        if (mask & candidates_[i])
                            eliminatedIndexes.push_back(i);
                    });
                }
                if (!eliminatedIndexes.empty())
                {
                    eliminatedValues.push_back(v);
                    pivots =
                    {
                        Board::Cell::indexOf(r0, c0),
                        Board::Cell::indexOf(r0, c1),
                        Board::Cell::indexOf(r1, c0),
                        Board::Cell::indexOf(r1, c1)
                    };
                    return true;
                }
            }
        }
    }
    return false;
}

bool Analyzer::xWingColumn(int                      c0,
                           std::vector<int> const & column,
                           std::vector<int> &       eliminatedIndexes,
                           std::vector<int> &       eliminatedValues,
                           std::vector<int> &       pivots) const
{
    StrongList links = findStrongLinks(column);
    for (auto link : links)
    {
        int r0        = link.u0;
        int r1        = link.u1;
        int v         = link.value;
        unsigned mask = candidate(v);

        for (int c1 = c0 + 1; c1 < Board::SIZE; ++c1)
        {
            std::vector<int> otherColumn = Board::Unit::column(c1);
            int other0 = otherColumn[r0];
            int other1 = otherColumn[r1];

            // If there is a strong link with the same value in the same columns in another column, then
            // this is an x-wing.
            if (hasStrongLink(other0, other1, mask, otherColumn))
            {
                // Look for other cells in these two rows with the value as a candidate
                for (int r : { r0, r1 })
                {
                    std::vector<int> row = Board::Unit::row(r);
                    for_each_index_except(row, row[c0], row[c1], [&](int i) {
                        if (mask & candidates_[i])
                            eliminatedIndexes.push_back(i);
                    });
                }
                if (!eliminatedIndexes.empty())
                {
                    eliminatedValues.push_back(v);
                    pivots =
                    {
                        Board::Cell::indexOf(r0, c0),
                        Board::Cell::indexOf(r0, c1),
                        Board::Cell::indexOf(r1, c0),
                        Board::Cell::indexOf(r1, c1)
                    };
                    return true;
                }
            }
        }
    }
    return false;
}

static std::string generateYWingReason(std::vector<int> const & pivots, std::vector<int> const & values)
{
    std::string reason = "If square " + Board::Cell::name(pivots[0]) +
                         " is " + std::to_string(values[0]) +
                         ", then square " + Board::Cell::name(pivots[1]) +
                         " must be " + std::to_string(values[2]) +
                         ", or if square " + Board::Cell::name(pivots[0]) +
                         " is " + std::to_string(values[1]) +
                         " then square " + Board::Cell::name(pivots[2]) +
                         " must be " + std::to_string(values[2]) +
                         ". Either way, none of these squares can be " + std::to_string(values[2]) +  ".";
        return reason;
}

bool Analyzer::yWingFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason) const
{
    // If a cell has a strong link of value A in a cell with only two candidates (A,C), and a strong link
    // of a value B in another cell with two candidates (B,C), and the other candidate in those two
    // cells are the value C, then any cells that can see both of those cells cannot have the value C
    // because one of those two cells, (A,C) or (B,C), must have the value C.

    std::vector<int> pivots;
    std::vector<int> pivotValues;

    bool found = !Board::ForEach::cell([&](int i) {
        // Get all the strong links for this cell
        StrongList links = findStrongLinks(i);

        if (!links.empty())
        {
            int i0 = i;
            for (StrongList::const_iterator link1 = links.begin(); link1 != std::prev(links.end()); ++link1)
            {
                int i1 = link1->i1;
                int v1 = link1->value;
                if (candidateCount(candidates_[i1]) == 2)
                {
                    for (StrongList::const_iterator link2 = std::next(link1); link2 != links.end(); ++link2)
                    {
                        int i2 = link2->i1;
                        int v2 = link2->value;
                        if (i1 != i2 && v1 != v2 && candidateCount(candidates_[i2]) == 2)
                        {
                            unsigned mask3 = (candidates_[i1] & ~candidate(v1)) & (candidates_[i2] & ~candidate(v2));
                            if (mask3)
                            {
                                assert(candidateCount(mask3) == 1);
                                // A Y-wing has been found. The candidate v3 can be removed from all cells that can
                                // see both i1 and i2.
                                int v3 = valueFromCandidates(mask3);
                                std::vector<int> seen = Board::Cell::dependents(i1, i2);
                                for (int s : seen)
                                {
                                    if (candidates_[s] & mask3)
                                        indexes.push_back(s);
                                }
                                if (!indexes.empty())
                                {
                                    values.push_back(v3);
                                    pivots = { i0, i1, i2 };
                                    pivotValues = { v1, v2, v3 };
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }); 

    if (found)
        reason = generateYWingReason(pivots, pivotValues);
    return found;
}

unsigned Analyzer::allCandidates(std::vector<int> const & indexes) const
{
    unsigned candidates = 0;
    for (int i : indexes)
    {
        unsigned c = candidates_[i];
        if (!solved(c))
            candidates |= c;
    }
    return candidates;
}

std::vector<Analyzer::StrongLink> Analyzer::findStrongLinks(std::vector<int> const & unit) const
{
    StrongList links;

    unsigned alreadyTested = 0;
    for (int u0 = 0; u0 < Board::SIZE - 1; ++u0)
    {
        int i0 = unit[u0];
        unsigned candidates0 = candidates_[i0];

        // Ignore solved cells
        if (solved(candidates0))
            continue;

        // Ignore cells with only already-tested candidate
        candidates0 &= ~alreadyTested;
        if (!candidates0)
            continue;

        std::vector<int> values = valuesFromCandidates(candidates0);
        for (int v : values)
        {
            unsigned mask = candidate(v);
            for (int u1 = u0 + 1; u1 < Board::SIZE; ++u1)
            {
                int i1 = unit[u1];
                if ((candidates_[i0] & candidates_[i1] & mask))
                {
                    if (hasStrongLinkR(u0, u1, mask, unit))
                        links.emplace_back(StrongLink { u0, i0, u1, i1, v });
                    break;
                }
            }
        }
        alreadyTested |= candidates0;
    }
    return links;
}

std::vector<Analyzer::StrongLink> Analyzer::findStrongLinks(int i) const
{
    StrongList links;

    std::vector<int> row = Board::Unit::row(Board::Unit::whichRow(i));
    StrongList rowLinks = findStrongLinks(i, row);
    links.insert(links.end(), rowLinks.begin(), rowLinks.end());

    std::vector<int> column = Board::Unit::column(Board::Unit::whichColumn(i));
    StrongList columnLinks = findStrongLinks(i, column);
    links.insert(links.end(), columnLinks.begin(), columnLinks.end());

    std::vector<int> box = Board::Unit::box(Board::Unit::whichBox(i));
    StrongList boxLinks = findStrongLinks(i, box);
    links.insert(links.end(), boxLinks.begin(), boxLinks.end());

    return links;
}

std::vector<Analyzer::StrongLink> Analyzer::findStrongLinks(int i0, std::vector<int> const & unit) const
{
    std::vector<Analyzer::StrongLink> links;
    std::vector<int> values = valuesFromCandidates(candidates_[i0]);
    for (int v : values)
    {
        unsigned mask = candidate(v);
        for_each_index_except(unit, i0, [&](int i1) {
            if (hasStrongLink(i0, i1, mask, unit))
                links.emplace_back(StrongLink { -1, i0, -1, i1, v });
        });
    }
    return links;
}

bool Analyzer::hasStrongLink(int i0, int i1, unsigned mask, std::vector<int> const & unit) const
{
    // Not a strong link if the two cells don't share the candidate
    if (!(candidates_[i0] & candidates_[i1] & mask))
        return false;

    // Not a strong link if any other cells in the unit share the candidate
    for (int i : unit)
    {
        if (i != i0 && i != i1 && (candidates_[i] & mask))
            return false;
    }
    return true;
}

bool Analyzer::hasStrongLinkR(int u0, int u1, unsigned mask, std::vector<int> const & unit) const
{
    // This is a faster version of hasStrongLink. This one requires that u0 < u1, cells u0 and u1 have candidates
    // corresponding to mask, and that no other cells in the given unit in the range [0, u1) have candidates
    // corresponding to mask.

#if defined(_DEBUG)
    {
        assert(u0 < u1);
        assert(candidates_[unit[u0]] & candidates_[unit[u1]] & mask);
        unsigned check = 0;
        for (int t = 0; t < u1; ++t)
        {
            if (t != u0)
                check |= candidates_[unit[t]];
        }
        assert(!(check & mask));
    }
#endif // if defined(_DEBUG)

    // Not a strong link if any of the remaining cells in the unit share the candidate
    for (int u2 = u1 + 1; u2 < Board::SIZE; ++u2)
    {
        int i2 = unit[u2];
        if ((candidates_[i2] & mask))
            return false;
    }
    return true;
}

#if defined(_DEBUG)
bool Analyzer::candidatesAreValid()
{
    return Board::ForEach::cell([&] (int i) {
        int v = solvedBoard_.get(i);
        assert(v != Board::EMPTY); // Sanity check
        return (candidate(v) & candidates_[i]) != 0;
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
        "y-wing"
    };
    assert((size_t)technique >= 0 && (size_t)technique < sizeof(NAMES) / sizeof(*NAMES));
    return NAMES[technique];
}
