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
    std::string serialized;
    board.serialize(serialized);
    puts(serialized.c_str());

    return 0;
}
