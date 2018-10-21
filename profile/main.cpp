#include "Board/Board.h"
#include "Generator/Generator.h"
#include "Solver/Solver.h"

#include <cstdio>
#include <ctime>

static int constexpr DEFAULT_NUMBER_OF_BOARDS = 1000;

static void ProfileGenerate(int count, std::vector<Board> & boards);
static void ProfileSolve(std::vector<Board> & boards);

int main(int argc, char ** argv)
{
    int count = DEFAULT_NUMBER_OF_BOARDS;

    --argc;
    ++argv;
    if (argc > 1)
    {
        fprintf(stderr, "syntax: profile [count]\n");
        return 1;
    }

    if (argc > 0)
    {
        count = atoi(*argv);
        if (count <= 0)
        {
            fprintf(stderr, "'%s' is an invalid count.\n", *argv);
            fprintf(stderr, "syntax: profile [count]\n");
            return 1;
        }
        --argc;
        ++argv;
    }

    srand((unsigned)time(NULL));
    std::vector<Board> boards;
    boards.reserve(count);

    ProfileGenerate(count, boards);
    ProfileSolve(boards);

    return 0;
}

static void ProfileGenerate(int count, std::vector<Board> & boards)
{
    printf("Profiling Generator::generate ...\n");

    time_t start_time = time(NULL);
    for (int i = 0; i < count; ++i)
    {
        boards.push_back(Generator::generate());
    }
    time_t end_time = time(NULL);

    int total_time = int(end_time - start_time);
    printf("%d boards\n", count);
    printf("total time = %d s\n", total_time);
    printf("average time = %g ms\n\n", float(total_time) / (float)count * 1000.0f);
}

static void ProfileSolve(std::vector<Board> & boards)
{
    printf("Profiling Solver::solve ...\n");
    time_t start_time = time(NULL);
    for (auto & b : boards)
    {
        Solver::solve(b);
    }
    time_t end_time = time(NULL);

    int total_time = int(end_time - start_time);
    printf("%d boards\n", (int)boards.size());
    printf("total time = %d s\n", total_time);
    printf("average time = %g ms\n\n", float(total_time) / (float)boards.size() * 1000.0f);
}
