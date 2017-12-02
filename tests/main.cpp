#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

int main(int argc, char ** argv)
{
//     --argc;
//     ++argv;
//     if (argc > 0 && argc < Board::SIZE * Board::SIZE)
//     {
//         fprintf(stderr, "syntax: soduko <81 values, 0-9>\n");
//         return 1;
//     }
//
//     LARGE_INTEGER counter;
//     QueryPerformanceCounter(&counter);
//     srand(counter.LowPart);
//
//     Board board;
//     if (argc > 0)
//     {
//         for (int r = 0; r < Board::SIZE; ++r)
//         {
//             for (int c = 0; c < Board::SIZE; ++c)
//             {
//                 int x;
//                 sscanf(*argv, "%d", &x);
//                 if (x < 0 || x > 9)
//                 {
//                     fprintf(stderr, "syntax: soduko <81 values, 0-9>\n");
//                     return 2;
//                 }
//                 board.set(r, c, x);
//                 --argc;
//                 ++argv;
//             }
//         }
//     }
//     else
//     {
//         board = Generator::generate();
//     }
//
//     printf("Board to solve:\n");
//     draw(board);
//     printf("Consistent: %s\n", board.consistent() ? "yes" : "no");
//     printf("Completed: %s\n", board.completed() ? "yes" : "no");
//     printf("Solved: %s\n", board.solved() ? "yes" : "no");
//
//     printf("\n");
//
//     std::vector<Board> solutions = Solver::allSolutions(board);
//     if (!solutions.empty())
//     {
//         printf("%d solutions.\n", (int)solutions.size());
//         printf("\n");
//         for (auto const & s : solutions)
//         {
//             printf("Solution:\n");
//             draw(s);
//             printf("Consistent: %s\n", s.consistent() ? "yes" : "no");
//             printf("Completed: %s\n", s.completed() ? "yes" : "no");
//             printf("Solved: %s\n", s.solved() ? "yes" : "no");
//             printf("\n");
//         }
//     }
//     else
//     {
//         printf("\n");
//         printf("No solutions.\n");
//     }
//
//     srand((unsigned)time(NULL));
//
//     std::vector<int> histo(81);
//
//     time_t start_time = time(NULL);
//     static int const NUMBER_OF_BOARDS = 1000;
//     for (int i = 0; i < NUMBER_OF_BOARDS; ++i)
//     {
//         Board b = Generator::generate();
//         int d   = b.difficulty();
//         ++histo[d];
//     }
//
//     time_t end_time = time(NULL);
//     int total_time  = int(end_time - start_time);
//     printf("total time = %d\n", total_time);
//     printf("average time = %g ms\n", float(total_time) / (float)NUMBER_OF_BOARDS * 1000.0f);
//
//     printf("Difficulty distribution:\n");
//     for (int i = 50; i < 70; ++i)
//     {
//         printf("%2d: %4d\n", i, histo[i]);
//     }

    return 0;
}
