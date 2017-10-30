#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

static void syntax()
{
    fprintf(stderr, "syntax: generate [difficulty]\n");
}

int main(int argc, char ** argv)
{
    --argc;
    ++argv;

    int difficulty = 0;

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
        syntax();
        return 1;
    }

    srand((unsigned int)time(NULL));

    Board board = Generator::generate(difficulty);
    printf("[ ");
    printf("%d", board.get(0));
    for (int i = 1; i < Board::SIZE*Board::SIZE; ++i)
    {
        printf(", %d", board.get(i));
    }
    printf(" ]\n");
    return 0;
}
