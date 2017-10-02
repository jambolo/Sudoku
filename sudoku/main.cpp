#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

static void draw(Board const & b)
{
    printf("+---+---+---+---+---+---+---+---+---+\n");
    for (int r = 0; r < Board::SIZE; ++r)
    {
        printf("|");
        for (int c = 0; c < Board::SIZE; ++c)
        {
            int x = b.get(r, c);
            if (x > 0)
            {
                printf(" %d |", x);
            }
            else
            {
                printf("   |");
            }
        }
        printf("\n+---+---+---+---+---+---+---+---+---+\n");
    }
}

int main(int argc, char ** argv)
{
    --argc;
    ++argv;
    if (argc > 0 && argc < Board::SIZE * Board::SIZE)
    {
        fprintf(stderr, "syntax: soduko <81 values, 0-9>\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    Board board;
    if (argc > 0)
    {
        for (int r = 0; r < Board::SIZE; ++r)
        {
            for (int c = 0; c < Board::SIZE; ++c)
            {
                int x;
                sscanf(*argv, "%d", &x);
                if (x < 0 || x > 9)
                {
                    fprintf(stderr, "syntax: soduko <81 values, 0-9>\n");
                    return 2;
                }
                board.set(r, c, x);
                --argc;
                ++argv;
            }
        }
    }
    else
    {
        board = Generator::generate();
    }

    printf("Board to solve:\n");
    draw(board);
    printf("Consistent: %s\n", board.consistent() ? "yes" : "no");
    printf("Completed: %s\n", board.completed() ? "yes" : "no");
    printf("Solved: %s\n", board.solved() ? "yes" : "no");
    printf("\n");

    std::vector<Board> solutions = Solver::allSolutions(board);
    if (!solutions.empty())
    {
        printf("%d solutions.\n", (int)solutions.size());
        printf("\n");
        for (auto const & s : solutions)
        {
            printf("Solution:\n");
            draw(s);
            printf("Consistent: %s\n", s.consistent() ? "yes" : "no");
            printf("Completed: %s\n", s.completed() ? "yes" : "no");
            printf("Solved: %s\n", s.solved() ? "yes" : "no");
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
