#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

int main(int argc, char ** argv)
{
    --argc;
    ++argv;

    int difficulty;

    if (argc == 0)
    {
        difficulty = 0;
    }
    else if (argc == 1)
    {
        sscanf(*argv, "%d", &difficulty);
    }
    else
    {
        fprintf(stderr, "syntax: generate [difficulty]\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    Board board = Generator::generate();
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            printf("%2d ", board.get(r, c));
        }
    }

    printf("\n");
    return 0;
}
