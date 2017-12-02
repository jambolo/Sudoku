#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

static void syntax()
{
    fprintf(stderr, "syntax: solve <81 values, 0-9>\n");
}

int main(int argc, char ** argv)
{
    --argc;
    ++argv;
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
            int x;
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

    printf("Board to solve:\n");
    board.draw();
    printf("\n");

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
