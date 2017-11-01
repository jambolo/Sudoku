#include "Analyzer/Analyzer.h"
#include "Board/Board.h"

#include <cstdio>
#include <ctime>

static void syntax()
{
    fprintf(stderr, "syntax: suggest [-v][-a] <81 values, 0-9>\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v: outputs additional information\n");
    fprintf(stderr, "  -a: output the complete solution.\n");
}

static void printStep(Analyzer::Step const & step, bool verbose, int i = 0)
{
    if (i > 0)
    {
        printf("%3d. ", i);
    }

    switch (step.action)
    {
        case Analyzer::Step::SOLVE:
        {
            int r;
            int c;
            Board::locationOf(step.indexes[0], &r, &c);
            printf("The value of %c%c is %d", Board::rowName(r), Board::columnName(c), step.values[0]);
            if (verbose)
            {
                printf(" (%s)", step.reason.c_str());
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
                printf("%c%c ", Board::rowName(r), Board::columnName(c));
            }
            if (verbose)
            {
                printf("(%s)", step.reason.c_str());
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
    bool verbose = false;
    bool all     = false;

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
        verbose = true;
        ++argv;
        --argc;
    }
    if (strcmp(*argv, "-v") == 0)
    {
        verbose = true;
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

    Analyzer analyzer(board);

    if (verbose)
    {
        printf("Board to solve:\n");
        analyzer.board().draw();
        printf("\n");
    }

    if (all)
    {
        int i = 1;
        while (!analyzer.done())
        {
            Analyzer::Step step = analyzer.next();
            printStep(step, verbose, i++);
        }
        printf("\n");
        analyzer.board().draw();
        printf("\n");
    }
    else
    {
        Analyzer analyzer(board);
        Analyzer::Step step = analyzer.next();
        printStep(step, verbose);
    }

    return 0;
}
