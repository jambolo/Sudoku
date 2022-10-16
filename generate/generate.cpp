#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

static void syntax()
{
    fprintf(stderr, "syntax: generate [max difficulty] [min difficulty]\n");
}

int main(int argc, char ** argv)
{
    --argc;
    ++argv;

    float minDifficulty = 0.0f;
    float maxDifficulty = 0.0f;

    if (argc == 0)
    {
        minDifficulty = 0.0f;
        maxDifficulty = 0.0f;
    }
    else if (argc == 1)
    {
        minDifficulty = 0.0f;
        sscanf(*argv, "%f", &maxDifficulty);
    }
    else if (argc == 2)
    {
        sscanf(*argv, "%f", &maxDifficulty);
        ++argv;
        sscanf(*argv, "%f", &minDifficulty);
    }
    else
    {
        syntax();
        return 1;
    }

    srand((unsigned int)time(NULL));

    Board       board = Generator::generate(maxDifficulty, minDifficulty);
    std::string serialized;
    board.serialize(serialized);
    puts(serialized.c_str());

    return 0;
}
