#include "Analyzer/Analyzer.h"
#include "Board/Board.h"

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
    fprintf(stderr, "syntax: suggest [-v][-a] <81 values, 0-9>\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v:   outputs additional information\n");
    fprintf(stderr, "  -vv:  outputs additional additional information\n");
    fprintf(stderr, "  -vvv: outputs additional additional additional information\n");
    fprintf(stderr, "  -a:   outputs the complete solution.\n");
}

static void printStep(Analyzer::Step const & step, Verbosity verbosity, int i = 0)
{
    if (i > 0)
        printf("%3d. ", i);

    switch (step.action)
    {
        case Analyzer::Step::SOLVE:
        {
            printf("The value of %s is %d", Board::locationName(step.indexes[0]).c_str(), step.values[0]);
            if (verbosity >= VERBOSE || verbosity >= DETAILED)
            {
                printf(" (%s)", Analyzer::Step::techniqueName(step.technique));
                if (verbosity >= DETAILED)
                    printf("\n     %s", step.reason.c_str());
            }
            printf("\n");
            break;
        }
        case Analyzer::Step::ELIMINATE:
        {
            for (auto v : step.values)
            {
                printf("%d ", v);
            }
            printf("can be eliminated as candidate values for ");
            for (auto v : step.indexes)
            {
                int r;
                int c;
                Board::locationOf(v, &r, &c);
                printf("%s ", Board::locationName(v).c_str());
            }
            if (verbosity >= VERBOSE || verbosity >= DETAILED)
            {
                printf(" (%s)", Analyzer::Step::techniqueName(step.technique));
                if (verbosity >= DETAILED)
                    printf("\n     %s", step.reason.c_str());
            }
            printf("\n");
            break;
        }
        case Analyzer::Step::STUCK:
            printf("Stuck\n");
            break;
        case Analyzer::Step::DONE:
            printf("Done\n");
            break;
    }
}

int main(int argc, char ** argv)
{
    bool all            = false;
    Verbosity verbosity = QUIET;

    --argc;
    ++argv;
    if (argc < Board::SIZE * Board::SIZE)
    {
        syntax();
        return 1;
    }

    if (strcmp(*argv, "-a") == 0)
    {
        all     = true;
        verbosity = VERBOSE;
        ++argv;
        --argc;
    }
    if (strcmp(*argv, "-v") == 0)
    {
        verbosity = VERBOSE;
        ++argv;
        --argc;
    }
    if (strcmp(*argv, "-vv") == 0)
    {
        verbosity = DETAILED;
        ++argv;
        --argc;
    }
    if (strcmp(*argv, "-vvv") == 0)
    {
        verbosity = CHATTY;
        ++argv;
        --argc;
    }

    if (argc != Board::SIZE * Board::SIZE)
    {
        syntax();
        return 1;
    }

    Board board;
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            int x = -1;
            sscanf(*argv, "%d", &x);
            if (x < 0 || x > 9)
            {
                syntax();
                return 2;
            }
            board.set(r, c, x);
            --argc;
            ++argv;
        }
    }

    if (!board.consistent())
    {
        fprintf(stderr, "The board is not valid.\n");
        return 3;
    }

    Analyzer analyzer(board, verbosity >= VERBOSE);

    if (verbosity >= VERBOSE)
    {
        printf("Board to solve:\n");
        if (verbosity >= DETAILED)
            analyzer.drawPenciledBoard();
        else
            analyzer.board().draw();
        printf("\n");
    }

    if (all)
    {
        int i = 1;
        do
        {
            Analyzer::Step step = analyzer.next();
            printStep(step, verbosity, i++);
            if (verbosity >= CHATTY && (step.action == Analyzer::Step::SOLVE || step.action == Analyzer::Step::ELIMINATE))
                analyzer.drawPenciledBoard();
        } while (!analyzer.done());

        printf("\n");
        if (analyzer.stuck() && verbosity >= DETAILED)
            analyzer.drawPenciledBoard();
        else
            analyzer.board().draw();
        printf("\n");
    }
    else
    {
        Analyzer::Step step;
        int i = 1;
        do
        {
            step = analyzer.next();
            printStep(step, verbosity, i++);
            if (verbosity >= CHATTY && step.action == Analyzer::Step::ELIMINATE)
                analyzer.drawPenciledBoard();
        } while (step.action == Analyzer::Step::ELIMINATE);

        printf("\n");
        if (verbosity >= DETAILED)
            analyzer.drawPenciledBoard();
        else if (verbosity >= VERBOSE)
            analyzer.board().draw();
    }

    return 0;
}
