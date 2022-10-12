#include "Analyzer/Analyzer.h"
#include "Board/Board.h"

#include <cmath>
#include <cstdio>

enum Verbosity
{
    QUIET,
    VERBOSE,
    DETAILED,
    CHATTY
};

static void syntax()
{
    fprintf(stderr, "syntax: rate [-vv] <81 digits, 0-9>\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v:   outputs additional information\n");
    fprintf(stderr, "  -vv:  outputs more additional information\n");
}

int main(int argc, char ** argv)
{
    Verbosity verbosity = QUIET;

    --argc;
    ++argv;

    while (**argv == '-')
    {
        if (strcmp(*argv, "-v") == 0)
        {
            verbosity = VERBOSE;
        }
        else if (strcmp(*argv, "-vv") == 0)
        {
            verbosity = DETAILED;
        }
        else
        {
            fprintf(stderr, "Invalid parameter '%s'\n", *argv);
            syntax();
            return 1;
        }

        ++argv;
        --argc;
    }

    if (argc < 1)
    {
        fprintf(stderr, "Missing board\n");
        syntax();
        return 2;
    }

    if (argc > 1)
    {
        fprintf(stderr, "Too many parameters\n");
        syntax();
        return 2;
    }

    if (strlen(*argv) < 81)
    {
        fprintf(stderr, "The board is missing squares\n");
        syntax();
        return 3;
    }

    if (strlen(*argv) > 81)
    {
        fprintf(stderr, "The board has too many squares\n");
        syntax();
        return 3;
    }

    Board board;
    if (!board.initialize(*argv))
    {
        fprintf(stderr, "The squares must be 0-9.\n");
        syntax();
        return 3;
    }

    if (!board.consistent())
    {
        fprintf(stderr, "The board is not valid. The squares are not consistent.\n");
        syntax();
        return 4;
    }

    Analyzer analyzer(board);

    if (verbosity >= VERBOSE)
    {
        printf("Board to solve:\n");
        analyzer.board().draw();
        printf("\n");
    }

    // Solve it, saving each step
    std::vector<Analyzer::Step> steps;
    do
    {
        steps.push_back(analyzer.next());
    } while (!analyzer.done());

    if (verbosity >= VERBOSE)
        printf("Number of steps: %d\n", (int)steps.size());

    // Report the number of times each technique is used
    if (verbosity >= DETAILED)
    {
        std::map<Analyzer::Step::TechniqueId, int> techniqueCounts;
        for (auto const & step : steps)
        {
            ++techniqueCounts[step.technique];
        }

        // We don't care about NONE steps
        techniqueCounts.erase(Analyzer::Step::NONE);

        printf("Technique usage:\n");
        for (auto const & entry : techniqueCounts)
        {
            Analyzer::Step::TechniqueId t = entry.first;
            int n = entry.second;
            printf("  %18s: %d\n", Analyzer::Step::techniqueName(t), n);
        }
        printf("\n");
    }

    float overallDifficulty;
    if (analyzer.stuck())
    {
        if (verbosity >= VERBOSE)
            printf("Sorry, I can't solve it.\n");
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
        // $$D \left( 1 + {1 \over 2}{{n_D - 1} \over n_D + 1} + {1 \over 2} \sum_{d=1}^{D-1} {{n_d \over {n_d+1}} {1 \over 2^{D-d}}} \right)$$
        // where _D_ is the highest difficulty, _n_ is the number of steps of difficulty _d_.
        //
        // The result is the highest difficulty plus up to 0.5 for additional steps of that difficulty, plus up to 0.5
        // for lower difficulty steps.

        overallDifficulty = float(highestDifficulty);
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

    if (verbosity >= VERBOSE)
        printf("Difficulty: ");
    printf("%.1f\n", overallDifficulty);

    return 0;
}
