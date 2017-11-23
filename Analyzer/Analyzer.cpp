#include "Analyzer.h"

#include "Board/Board.h"

#include <algorithm>
#include <cassert>
#include <numeric>

static int const ALL_CANDIDATES = 0x3fe;

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

Analyzer::Analyzer(Board const & board, bool verbose /*= false*/)
    : board_(board)
    , verbose_(verbose)
    , unsolved_(Board::SIZE * Board::SIZE)
    , candidates_(Board::SIZE * Board::SIZE, ALL_CANDIDATES)
    , done_(false)
{
    // Nothing is solved initially
    std::iota(unsolved_.begin(), unsolved_.end(), 0);

    // For solved squares, mark as solved
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            if (!board.isEmpty(r, c))
            {
                int x = board.get(r, c);
                solve(r, c, x);
            }
        }
    }
}

Analyzer::Step Analyzer::next()
{
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
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE, indexes, values, Step::NAKED_SINGLE, reason };
    }

    if (hiddenSingleFound(indexes, values, reason))
    {
        int r;
        int c;
        Board::locationOf(indexes.front(), &r, &c);
        solve(r, c, values.front());
        return { Step::SOLVE, indexes, values, Step::HIDDEN_SINGLE, reason };
    }

    if (nakedPairFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::NAKED_PAIR, reason };
    }

    if (nakedTripleFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::NAKED_TRIPLE, reason };
    }

    if (nakedQuadFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::NAKED_QUAD, reason };
    }

    if (lockedCandidatesFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::LOCKED_CANDIDATES, reason };
    }

    if (hiddenPairFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_PAIR, reason };
    }

    if (hiddenTripleFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_TRIPLE, reason };
    }

    if (hiddenQuadFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::HIDDEN_QUAD, reason };
    }

    if (xWingFound(indexes, values, reason))
    {
        eliminate(indexes, values);
        return { Step::ELIMINATE, indexes, values, Step::X_WING, reason };
    }

    done_ = true;
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
            unsigned candidates = candidates_[Board::indexOf(r, c)];
            if (solved(candidates))
            {
                printf("       ");
            }
            else
            {
                char x1 = (candidates & (1 << 1)) ? '1' : '.';
                char x2 = (candidates & (1 << 2)) ? '2' : '.';
                char x3 = (candidates & (1 << 3)) ? '3' : '.';
                printf(" %c %c %c ", x1, x2, x3);
            }
            if (c % 3 == 2)
                printf("|");
            else
                printf(":");
        }
        printf("\n");

        printf("%c |", Board::rowName(r));
        for (int c = 0; c < Board::SIZE; ++c)
        {
            unsigned candidates = candidates_[Board::indexOf(r, c)];
            if (solved(candidates))
            {
                int v = board_.get(r, c);
                printf("   %d   ", v);
            }
            else
            {
                char x4 = (candidates & (1 << 4)) ? '4' : '.';
                char x5 = (candidates & (1 << 5)) ? '5' : '.';
                char x6 = (candidates & (1 << 6)) ? '6' : '.';
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
            unsigned candidates = candidates_[Board::indexOf(r, c)];
            if (solved(candidates))
            {
                printf("       ");
            }
            else
            {
                char x7 = (candidates & (1 << 7)) ? '7' : '.';
                char x8 = (candidates & (1 << 8)) ? '8' : '.';
                char x9 = (candidates & (1 << 9)) ? '9' : '.';
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

void Analyzer::solve(int r, int c, int x)
{
    // Update the board
    board_.set(r, c, x);

    // The square has only one candidate now
    int index = Board::indexOf(r, c);
    candidates_[index] = 1 << x;

    // Remove from the list of unsolved squares
    unsolved_.erase(std::remove(unsolved_.begin(), unsolved_.end(), index), unsolved_.end());

    // Eliminate this square's value from its dependents' candidates
    std::vector<int> dependents = Board::getDependents(r, c);
    eliminate(dependents, x);
}

void Analyzer::eliminate(std::vector<int> const & indexes, int x)
{
    for (auto i : indexes)
    {
        candidates_[i] &= ~(1 << x);
        assert(candidates_[i] != 0);
    }
}

void Analyzer::eliminate(std::vector<int> const & indexes, std::vector<int> const & values)
{
    for (auto v : values)
    {
        eliminate(indexes, v);
    }
}

bool Analyzer::hiddenSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    bool found;
    int which;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = std::string("This is the only square in row ") + Board::rowName(which) + " that can have this value.";
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = std::string("This is the only square in column ") + Board::columnName(which) + " that can have this value.";
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = std::string("This is the only square in box ") + Board::boxName(which) + " that can have this value.";
        return true;
    }

    return false;
}

bool Analyzer::hiddenSingle(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues)
{
    for (int s : indexes)
    {
        if (board_.isEmpty(s))
        {
            unsigned others = 0;
            for (int i : indexes)
            {
                if (i != s)
                    others |= candidates_[i];
            }

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

bool Analyzer::hiddenPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive pair in a unit, if they have additional candidates, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = generateHiddenPairReason("row", Board::rowName(which), hidden);
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = generateHiddenPairReason("column", Board::columnName(which), hidden);
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = generateHiddenPairReason("box", Board::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Analyzer::hiddenPair(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues,
                          std::vector<int> &       hiddenValues)
{
    // Go through each possible pair of candidates and search for exactly two cells containing the one or more of a pair
    for (int x0 = 1; x0 <= Board::SIZE - 1; ++x0)
    {
        unsigned m0 = 1 << x0;
        for (int x1 = x0 + 1; x1 <= Board::SIZE; ++x1)
        {
            unsigned m1 = 1 << x1;
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

bool Analyzer::hiddenTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive triple in a unit, if they have additional candidates, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = generateHiddenTripleReason("row", Board::rowName(which), hidden);
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = generateHiddenTripleReason("column", Board::columnName(which), hidden);
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = generateHiddenTripleReason("box", Board::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Analyzer::hiddenTriple(std::vector<int> const & indexes,
                            std::vector<int> &       eliminatedIndexes,
                            std::vector<int> &       eliminatedValues,
                            std::vector<int> &       hiddenValues)
{
    // Go through each possible triple of candidates and search for exactly three cells containing the one or more of a triple
    for (int x0 = 1; x0 <= Board::SIZE - 2; ++x0)
    {
        unsigned m0 = 1 << x0;
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 1; ++x1)
        {
            unsigned m1 = 1 << x1;
            for (int x2 = x1 + 1; x2 <= Board::SIZE; ++x2)
            {
                unsigned m2 = 1 << x2;
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

bool Analyzer::hiddenQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive quad in a unit, if they have additional candidates, then success.

    bool found;
    int which;
    std::vector<int> hidden;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = generateHiddenQuadReason("row", Board::rowName(which), hidden);
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = generateHiddenQuadReason("column", Board::columnName(which), hidden);
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = generateHiddenQuadReason("box", Board::boxName(which), hidden);
        return true;
    }

    return false;
}

bool Analyzer::hiddenQuad(std::vector<int> const & indexes,
                          std::vector<int> &       eliminatedIndexes,
                          std::vector<int> &       eliminatedValues,
                          std::vector<int> &       hiddenValues)
{
    // Go through each possible quad of candidates and search for exactly four cells containing the one or more of a quad
    for (int x0 = 1; x0 <= Board::SIZE - 3; ++x0)
    {
        unsigned m0 = 1 << x0;
        for (int x1 = x0 + 1; x1 <= Board::SIZE - 2; ++x1)
        {
            unsigned m1 = 1 << x1;
            for (int x2 = x1 + 1; x2 <= Board::SIZE - 1; ++x2)
            {
                unsigned m2 = 1 << x2;
                for (int x3 = x2 + 1; x3 <= Board::SIZE; ++x3)
                {
                    unsigned m3 = 1 << x3;
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

bool Analyzer::nakedSingleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    if (nakedSingle(indexes, values))
    {
        if (verbose_)
            reason = "There are no other possible values for this square.";
        return true;
    }
    return false;
}

bool Analyzer::nakedSingle(std::vector<int> & indexes, std::vector<int> & values)
{
    // For each unsolved square, if it only has one candidate, then success
    for (auto i : unsolved_)
    {
        unsigned candidates = candidates_[i];
        if (solved(candidates))
        {
            indexes.push_back(i);
            values.push_back(valueFromCandidates(candidates));
            return true;
        }
    }
    return false;
}

static std::string generateNakedPairReason(std::string const & unitType, char which, std::vector<int> const & nakedIndexes)
{
    std::string reason = "Two other squares (" + Board::locationName(nakedIndexes[0]) +
                         " and " + Board::locationName(nakedIndexes[1]) +
                         ") in " + unitType + " " + which +
                         " must be one of these two values, so these squares cannot";
    return reason;
}

bool Analyzer::nakedPairFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive pair in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = generateNakedPairReason("row", Board::rowName(which), nakedIndexes);
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = generateNakedPairReason("column", Board::columnName(which), nakedIndexes);
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = generateNakedPairReason("box", Board::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Analyzer::nakedPair(std::vector<int> const & indexes,
                         std::vector<int> &       eliminatedIndexes,
                         std::vector<int> &       eliminatedValues,
                         std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 1; ++b0)
    {
        int i0     = indexes[b0];
        int count0 = candidateCount(candidates_[i0]);
        if (count0 > 1 && count0 <= 2)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE; ++b1)
            {
                int i1     = indexes[b1];
                int count1 = candidateCount(candidates_[i1]);
                if (count1 > 1 && count1 <= 2)
                {
                    unsigned candidates = candidates_[i0] | candidates_[i1];
                    if (candidateCount(candidates) == 2)
                    {
                        for (auto i : indexes)
                        {
                            if (i != i0 && i != i1)
                            {
                                unsigned eliminated = candidates_[i] & candidates;
                                if (eliminated)
                                    eliminatedIndexes.push_back(i);
                            }
                        }
                        if (!eliminatedIndexes.empty())
                        {
                            eliminatedValues = valuesFromCandidates(candidates);
                            nakedIndexes     = { i0, i1 };
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

static std::string generateNakedTripleReason(std::string const & unitType, char which, std::vector<int> const & nakedIndexes)
{
    std::string reason = std::string("Three other squares (") + Board::locationName(nakedIndexes[0]) +
                         ", " + Board::locationName(nakedIndexes[1]) +
                         " and " + Board::locationName(nakedIndexes[2]) +
                         ") in " + unitType + " " + which +
                         " must be one of these three values, so these squares cannot";
    return reason;
}

bool Analyzer::nakedTripleFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive triple in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = generateNakedTripleReason("row", Board::rowName(which), nakedIndexes);
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = generateNakedTripleReason("column", Board::columnName(which), nakedIndexes);
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = generateNakedTripleReason("box", Board::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Analyzer::nakedTriple(std::vector<int> const & indexes,
                           std::vector<int> &       eliminatedIndexes,
                           std::vector<int> &       eliminatedValues,
                           std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 2; ++b0)
    {
        int i0     = indexes[b0];
        int count0 = candidateCount(candidates_[i0]);
        if (count0 > 1 && count0 <= 3)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 1; ++b1)
            {
                int i1     = indexes[b1];
                int count1 = candidateCount(candidates_[i1]);
                if (count1 > 1 && count1 <= 3)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE; ++b2)
                    {
                        int i2     = indexes[b2];
                        int count2 = candidateCount(candidates_[i2]);
                        if (count2 > 1 && count2 <= 3)
                        {
                            unsigned candidates = candidates_[i0] | candidates_[i1] | candidates_[i2];
                            if (candidateCount(candidates) == 3)
                            {
                                for (auto i : indexes)
                                {
                                    if (i != i0 && i != i1 && i != i2)
                                    {
                                        unsigned eliminated = candidates_[i] & candidates;
                                        if (eliminated)
                                            eliminatedIndexes.push_back(i);
                                    }
                                }
                                if (!eliminatedIndexes.empty())
                                {
                                    eliminatedValues = valuesFromCandidates(candidates);
                                    nakedIndexes     = { i0, i1, i2 };
                                    return true;
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

static std::string generateNakedQuadReason(std::string const & unitType, char which, std::vector<int> const & nakedIndexes)
{
    std::string reason = "Four other squares (" + Board::locationName(nakedIndexes[0]) +
                         ", " + Board::locationName(nakedIndexes[1]) +
                         ", " + Board::locationName(nakedIndexes[2]) +
                         " and " + Board::locationName(nakedIndexes[3]) +
                         ") in " + unitType + " " + which +
                         " must be one of these four values, so these squares cannot";
    return reason;
}

bool Analyzer::nakedQuadFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive quad in a unit, if there are other candidates that overlap, then success.

    bool found;
    int which;
    std::vector<int> nakedIndexes;

    found = !board_.for_each_row([&] (int r, std::vector<int> const & row) {
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
            reason = generateNakedQuadReason("row", Board::rowName(which), nakedIndexes);
        return true;
    }

    found = !board_.for_each_column([&] (int c, std::vector<int> const & column) {
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
            reason = generateNakedQuadReason("column", Board::columnName(which), nakedIndexes);
        return true;
    }

    found = !board_.for_each_box([&] (int b, std::vector<int> const & box) {
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
            reason = generateNakedQuadReason("box", Board::boxName(which), nakedIndexes);
        return true;
    }

    return false;
}

bool Analyzer::nakedQuad(std::vector<int> const & indexes,
                         std::vector<int> &       eliminatedIndexes,
                         std::vector<int> &       eliminatedValues,
                         std::vector<int> &       nakedIndexes)
{
    for (int b0 = 0; b0 < Board::SIZE - 3; ++b0)
    {
        int i0     = indexes[b0];
        int count0 = candidateCount(candidates_[i0]);
        if (count0 > 1 && count0 <= 4)
        {
            for (int b1 = b0 + 1; b1 < Board::SIZE - 2; ++b1)
            {
                int i1     = indexes[b1];
                int count1 = candidateCount(candidates_[i1]);
                if (count1 > 1 && count1 <= 4)
                {
                    for (int b2 = b1 + 1; b2 < Board::SIZE - 1; ++b2)
                    {
                        int i2     = indexes[b2];
                        int count2 = candidateCount(candidates_[i2]);
                        if (count2 > 1 && count2 <= 4)
                        {
                            for (int b3 = b2 + 1; b3 < Board::SIZE; ++b3)
                            {
                                int i3     = indexes[b3];
                                int count3 = candidateCount(candidates_[i3]);
                                if (count3 > 1 && count3 <= 4)
                                {
                                    unsigned candidates = candidates_[i0] | candidates_[i1] | candidates_[i2] | candidates_[i3];
                                    if (candidateCount(candidates) == 4)
                                    {
                                        for (auto i : indexes)
                                        {
                                            if (i != i0 && i != i1 && i != i2 && i != i3)
                                            {
                                                unsigned eliminated = candidates_[i] & candidates;
                                                if (eliminated)
                                                    eliminatedIndexes.push_back(i);
                                            }
                                        }
                                        if (!eliminatedIndexes.empty())
                                        {
                                            eliminatedValues = valuesFromCandidates(candidates);
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

bool Analyzer::lockedCandidatesFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the row/column, then success if those candidates exist in the box.

    bool found;
    int which1, which2;

    found = !board_.for_each_row([&](int r, std::vector<int> const & row) {
        for (int c = 0; c < Board::SIZE; c += Board::BOX_SIZE)
        {
            int b = Board::indexOfBox(r, c);
            std::vector<int> box = Board::getBoxIndexes(b);
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
        reason = generateLockedCandidatesReason("row", Board::rowName(which1), "box", Board::boxName(which2));
        return true;
    }

    found = !board_.for_each_column([&](int c, std::vector<int> const & column) {
        for (int r = 0; r < Board::SIZE; r += Board::BOX_SIZE)
        {
            int b = Board::indexOfBox(r, c);
            std::vector<int> box = Board::getBoxIndexes(b);
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
            reason = generateLockedCandidatesReason("column", Board::columnName(which1), "box", Board::boxName(which2));
        return true;
    }

    // For the intersection of each row or column with a box, if there are candidates that exist within the
    // intersection but not in the rest of the box, then success if those candidates exist in the row/column.

    found = !board_.for_each_row([&](int r, std::vector<int> const & row) {
        for (int c = 0; c < Board::SIZE; c += Board::BOX_SIZE)
        {
            int b = Board::indexOfBox(r, c);
            std::vector<int> box = Board::getBoxIndexes(b);
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
            reason = generateLockedCandidatesReason("box", Board::boxName(which1), "row", Board::rowName(which2));
        return true;
    }

    found = !board_.for_each_column([&](int c, std::vector<int> const & column) {
        for (int r = 0; r < Board::SIZE; r += Board::BOX_SIZE)
        {
            int b = Board::indexOfBox(r, c);
            std::vector<int> box = Board::getBoxIndexes(b);
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
            reason = generateLockedCandidatesReason("box", Board::boxName(which1), "row", Board::columnName(which2));
        return true;
    }

    return false;
}

bool Analyzer::lockedCandidates(std::vector<int> const & indexes1,
                                std::vector<int> const & indexes2,
                                std::vector<int> &       eliminatedIndexes,
                                std::vector<int> &       eliminatedValues)
{
    // Indexes in the intersection
    std::vector<int> intersection(Board::BOX_SIZE);
    std::set_intersection(indexes1.begin(), indexes1.end(), indexes2.begin(), indexes2.end(), intersection.begin());

    // Indexes not in the intersection
    std::vector<int> others1(Board::SIZE - Board::BOX_SIZE);
    std::set_difference(indexes1.begin(), indexes1.end(), intersection.begin(), intersection.end(), others1.begin());
    std::vector<int> others2(Board::SIZE - Board::BOX_SIZE);
    std::set_difference(indexes2.begin(), indexes2.end(), intersection.begin(), intersection.end(), others2.begin());

    // Candidates in the intersection
    unsigned intersectionCandidates = 0;
    for (int i : intersection)
    {
        if (!solved(candidates_[i]))
            intersectionCandidates |= candidates_[i];
    }

    // Candidates in set 1, not in intersection
    unsigned otherCandidates1 = 0;
    for (int i : others1)
    {
        if (!solved(candidates_[i]))
            otherCandidates1 |= candidates_[i];
    }

    // If any of the candidates in the intersection don't exist in the rest of set 1, then eliminate them from set 2
    unsigned unique1 = intersectionCandidates & ~otherCandidates1;
    if (unique1)
    {
        bool found = false;
        for (int i : others2)
        {
            if (candidates_[i] & unique1)
            {
                eliminatedIndexes.push_back(i);
                found = true;
            }
        }
        if (found)
        {
            std::vector<int> uniqueCandidates = valuesFromCandidates(unique1);
            eliminatedValues.insert(eliminatedValues.end(), uniqueCandidates.begin(), uniqueCandidates.end());
        }
    }

    std::sort(eliminatedIndexes.begin(), eliminatedIndexes.end());
    eliminatedIndexes.erase(std::unique(eliminatedIndexes.begin(), eliminatedIndexes.end()), eliminatedIndexes.end());
    std::sort(eliminatedValues.begin(), eliminatedValues.end());
    eliminatedValues.erase(std::unique(eliminatedValues.begin(), eliminatedValues.end()), eliminatedValues.end());

    return !eliminatedIndexes.empty();
}

bool Analyzer::xWingFound(std::vector<int> & indexes, std::vector<int> & values, std::string & reason)
{
    // For each exclusive pair in a unit, if they have additional candidates, then success.

    bool found;

    found = !board_.for_each_row([&](int r, std::vector<int> const & row) {
        return !xWing(row, indexes, values);
    });
    if (found)
    {
        reason = "two squares in a row have exclusive candidate pairs that correspond to candidates in another row in the same columns, so other squares in those columns cannot have these values";
        return true;
    }

    found = !board_.for_each_column([&](int c, std::vector<int> const & column) {
        return !xWing(column, indexes, values);
    });
    if (found)
    {
        reason = "two squares in a column have exclusive candidate pairs that correspond to candidates in another column in the same rows, so other squares in those rows cannot have these values";
        return true;
    }

    found = !board_.for_each_box([&](int b, std::vector<int> const & box) {
        return !xWing(box, indexes, values);
    });
    if (found)
    {
        reason = "two squares in a box have exclusive candidate pairs that correspond to candidates in another box, so other squares in those boxes cannot have these values";
        return true;
    }

    return false;
}

bool Analyzer::xWing(std::vector<int> const & indexes,
    std::vector<int> & eliminatedIndexes,
    std::vector<int> & eliminatedValues,
    int * otherUnit,
    std::vector<int> pivots)
{
    return false;
}

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
        "x wing"
    };
    assert((size_t)technique >= 0 && (size_t)technique < sizeof(NAMES) / sizeof(*NAMES));
    return NAMES[technique];
}
