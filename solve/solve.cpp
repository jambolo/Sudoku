#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

static void syntax()
{
    fprintf(stderr, "syntax: solve <81 digits, 0-9>\n");
}

int main(int argc, char ** argv)
{
    --argc;
    ++argv;

    if (argc < 1)
    {
        fprintf(stderr, "Missing board.\n");
        syntax();
        return 2;
    }

    if (argc > 1)
    {
        fprintf(stderr, "Too many parameters.\n");
        syntax();
        return 2;
    }

    if (strlen(*argv) < 81)
    {
        fprintf(stderr, "The board is missing squares.\n");
        syntax();
        return 3;
    }

    if (strlen(*argv) > 81)
    {
        fprintf(stderr, "The board has too many squares.\n");
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

    printf("Board to solve:\n");
    board.draw();
    printf("\n");

    if (!board.consistent())
    {
        fprintf(stderr, "The board is not valid. The squares are not consistent.\n");
        syntax();
        return 4;
    }

    std::vector<Board> solutions = Solver::allSolutions(board);
    if (!solutions.empty())
    {
        printf("%d solutions.\n", (int)solutions.size());
        printf("\n");
        for (auto const & s : solutions)
        {
            s.draw();
            printf("\n");
        }
    }
    else
    {
        printf("\n");
        printf("No solutions.\n");
    }

    return 0;
}
