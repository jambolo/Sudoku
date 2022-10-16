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

    if (argc == 1)
    {
        minDifficulty = 0.0f;
        (void)sscanf(*argv, "%f", &maxDifficulty);
        if (maxDifficulty < 0.0f)
        {
            fprintf(stderr, "generate: The maximum difficulty must be at least 0.\n");
            return 1;
        }
    }
    else if (argc == 2)
    {
        (void)sscanf(*argv, "%f", &maxDifficulty);
        if (maxDifficulty <= 0.0f)
        {
            fprintf(stderr, "generate: The maximum difficulty must be at least 0.\n");
            return 1;
        }
        ++argv;
        (void)sscanf(*argv, "%f", &minDifficulty);
        if (minDifficulty < 0.0f)
        {
            fprintf(stderr, "generate: The minimum difficulty must be at least 0.\n");
            return 1;
        }
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
