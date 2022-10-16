#include "Generator.h"

#include "Analyzer/Analyzer.h"
#include "Board/Board.h"
#include "Solver/Solver.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#if !defined(XCODE_COMPATIBLE_ASSERT)
#if defined(_DEBUG)
#define XCODE_COMPATIBLE_ASSERT assert
#else
#define XCODE_COMPATIBLE_ASSERT(...)
#endif
#endif // !defined(XCODE_COMPATIBLE_ASSERT)

Board Generator::generate(float maxDifficulty /* = 0.0f*/, float minDifficulty /* = 0.0f*/)
{
    float difficulty = 0.0f;
    Board board;

    // Generate a random solved boards until we find one with the right difficulty
    do
    {
        board = generateSolvedBoard();

        // Randomly remove as many cells as possible
        std::vector<int> indexes = randomizedIndexes();
        for (auto i : indexes)
        {
            // Remove a cell
            int x = board.get(i);
            board.set(i, Board::EMPTY);

            // If the new puzzle doesn't have a unique solution, then undo and try again
            if (!Solver::hasUniqueSolution(board))
            {
                board.set(i, x); // Skip this one
                continue;
            }

            // If it is too difficult, then undo and try again
            float newDifficulty = computeDifficulty(board);
            if (maxDifficulty > 0.0f && newDifficulty >= maxDifficulty + 1.0f)
            {
                board.set(i, x); // Skip this one
                continue;
            }
            else
            {
                difficulty = newDifficulty;
            }
        }
    } while (difficulty < minDifficulty);

    return board;
}

Board Generator::generateSolvedBoard()
{
    Board board;
    bool  successful = attempt(board, 0);
    XCODE_COMPATIBLE_ASSERT(successful);
    return board;
}

bool Generator::attempt(Board & board, int i)
{
    // This function attempts to find a solution recursively by attempting all possible values for the specified cell in random
    // order until values can be found for all of the following cells.

    // If there are no remaining cells to try, then the board has been generated
    if (i >= Board::NUM_CELLS)
        return true;

    // Generate all the candidates for this cell. If there are none, then the board is not viable.
    std::vector<int> possibleValues = board.candidates(i);
    if (possibleValues.empty())
        return false;

    std::random_shuffle(possibleValues.begin(), possibleValues.end());
    for (auto x : possibleValues)
    {
        board.set(i, x);

        // Try to fill in the remaining cells. If that succeeds, then the board has been generated
        if (attempt(board, i + 1))
            return true;
    }

    // None of the candidates for this cell worked, so it isn't viable
    board.set(i, Board::EMPTY); // Undo any attempt in this cell
    return false;
}

std::vector<int> Generator::randomizedIndexes()
{
    std::vector<int> indexes(Board::NUM_CELLS);
    std::iota(indexes.begin(), indexes.end(), 0);
    std::random_shuffle(indexes.begin(), indexes.end());
    return indexes;
}

float Generator::computeDifficulty(Board const & board)
{
    Analyzer analyzer(board);

    // Solve it, saving each step
    std::vector<Analyzer::Step> steps;
    do
    {
        steps.push_back(analyzer.next());
    } while (!analyzer.done());

    float overallDifficulty;
    if (analyzer.stuck())
    {
        overallDifficulty = 9001.0f;
    }
    else
    {
        // Count the number of times a step of each difficulty is used and find the highest difficulty
        std::map<int, int> difficultyCounts;
        int highestDifficulty = 0;
        for (auto const & step : steps)
        {
            if (step.technique != Analyzer::Step::NONE)
            {
                int difficulty = Analyzer::Step::techniqueDifficulty(step.technique);
                ++difficultyCounts[difficulty];
                if (difficulty > highestDifficulty)
                    highestDifficulty = difficulty;
            }
        }

        // Overall difficulty is computed as follows:
        //
        // $$D \left( 1 + {1 \over 2}{{n_D - 1} \over n_D + 1} + {1 \over 2} \sum_{d=1}^{D-1} {{n_d \over {n_d+1}} {1 \over
        // 2^{D-d}}} \right)$$
        // where _D_ is the highest difficulty, _n_ is the number of steps of difficulty _d_.
        //
        // The result is the highest difficulty plus up to 0.5 for additional steps of that difficulty, plus up to 0.5 for lower
        // difficulty steps.

        overallDifficulty  = float(highestDifficulty);
        overallDifficulty -= 0.5f / float(difficultyCounts[highestDifficulty] + 1);
        for (auto const & entry : difficultyCounts)
        {
            int d = entry.first;
            int n = entry.second;
            XCODE_COMPATIBLE_ASSERT(d > 0);
            XCODE_COMPATIBLE_ASSERT(n > 0);
            float factor = float(n) / float(n + 1) * powf(2.0f, float(d - highestDifficulty - 1));
            overallDifficulty += factor;
        }
    }
    return overallDifficulty;
}
