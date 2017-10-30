#include "Board/Board.h"
#include "Analyzer/Analyzer.h"

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

    switch (step.type)
    {
    case Analyzer::Step::SOLVE:
    {
        int r;
        int c;
        Board::locationOf(step.indexes[0], &r, &c);
        printf("The value of %c%d is %d", 'A' + r, 1 + c, step.value);
        if (verbose)
        {
            printf(" (%s)", step.reason.c_str());
        }
        printf("\n");
        break;
    }
    case Analyzer::Step::ELIMINATE:
        break;
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
    bool all = false;

    --argc;
    ++argv;
    if (argc < Board::SIZE * Board::SIZE)
    {
        syntax();
        return 1;
    }

    if (strcmp(*argv, "-a") == 0)
    {
        all = true;
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
