#include "Analyzer/Analyzer.h"
#include "Board/Board.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <cstdio>

enum Verbosity
{
    QUIET,
    JSON,
    VERBOSE,
    DETAILED,
    CHATTY
};

static json s_json;

static void syntax()
{
    fprintf(stderr, "syntax: suggest [-a] [-vvv | -j] <81 digits, 0-9>\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v:   outputs additional information\n");
    fprintf(stderr, "  -vv:  outputs additional additional information\n");
    fprintf(stderr, "  -vvv: outputs additional additional additional information\n");
    fprintf(stderr, "  -a:   outputs the complete solution.\n");
    fprintf(stderr, "  -j:   outputs information in json format.\n");
}

static void printStep(Analyzer::Step const & step, Verbosity verbosity, int i = 0)
{
    if (i > 0)
        printf("%3d. ", i);

    switch (step.action)
    {
        case Analyzer::Step::SOLVE:
        {
            printf("The value of %s is %d", Board::Cell::name(step.indexes[0]).c_str(), step.values[0]);
            if (verbosity >= VERBOSE || verbosity >= DETAILED)
            {
                printf(" (%s)", Analyzer::Step::techniqueName(step.technique));
                if (verbosity >= DETAILED)
                    printf("\n     %s", step.reason.c_str());
            }
            printf("\n");
            break;
        }
        case Analyzer::Step::ELIMINATE:
        {
            for (auto v : step.values)
            {
                printf("%d ", v);
            }
            printf("can be eliminated as candidate values for ");
            for (auto v : step.indexes)
            {
                printf("%s ", Board::Cell::name(v).c_str());
            }
            if (verbosity >= VERBOSE || verbosity >= DETAILED)
            {
                printf(" (%s)", Analyzer::Step::techniqueName(step.technique));
                if (verbosity >= DETAILED)
                    printf("\n     %s", step.reason.c_str());
            }
            printf("\n");
            break;
        }
        case Analyzer::Step::STUCK:
            printf("Stuck\n");
            break;
        case Analyzer::Step::DONE:
            printf("Done\n");
            break;
    }
}

int main(int argc, char ** argv)
{
    bool all = false;
    Verbosity verbosity = QUIET;

    --argc;
    ++argv;

    while (**argv == '-')
    {
        if (strcmp(*argv, "-a") == 0)
        {
            all = true;
        }
        else if (strcmp(*argv, "-v") == 0)
        {
            verbosity = VERBOSE;
        }
        else if (strcmp(*argv, "-vv") == 0)
        {
            verbosity = DETAILED;
        }
        else if (strcmp(*argv, "-vvv") == 0)
        {
            verbosity = CHATTY;
        }
        else if (strcmp(*argv, "-j") == 0)
        {
            verbosity = JSON;
        }
        else
        {
            fprintf(stderr, "Invalid parameter '%s'\n", *argv);
            syntax();
            return 1;
        }

        ++argv;
        --argc;
    }

    if (argc < 1)
    {
        fprintf(stderr, "Missing board\n");
        syntax();
        return 2;
    }

    if (argc > 1)
    {
        fprintf(stderr, "Too many parameters\n");
        syntax();
        return 2;
    }

    if (strlen(*argv) < 81)
    {
        fprintf(stderr, "The board is missing squares\n");
        syntax();
        return 3;
    }

    if (strlen(*argv) > 81)
    {
        fprintf(stderr, "The board has too many squares\n");
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

    if (!board.consistent())
    {
        fprintf(stderr, "The board is not valid. The squares are not consistent.\n");
        syntax();
        return 4;
    }

    Analyzer analyzer(board);

    if (verbosity >= JSON)
    {
        s_json["start"] = analyzer.board().toJson();
        s_json["steps"] = json();
    }
    else
    {
        if (verbosity >= VERBOSE)
        {
            printf("Board to solve:\n");
            if (verbosity >= DETAILED)
                analyzer.drawCandidates();
            else
                analyzer.board().draw();
            printf("\n");
        }
    }

    if (all)
    {
        int i = 1;
        do
        {
            Analyzer::Step step = analyzer.next();
            if (verbosity == JSON)
            {
                s_json["steps"].emplace_back(step.toJson());
            }
            else
            {
                printStep(step, verbosity, i++);
                if (verbosity >= CHATTY && (step.action == Analyzer::Step::SOLVE || step.action == Analyzer::Step::ELIMINATE))
                    analyzer.drawCandidates();
            }
        } while (!analyzer.done());

        if (verbosity == JSON)
        {
            s_json["end"] = analyzer.board().toJson();
            printf("%s\n", s_json.dump().c_str());
        }
        else
        {
            printf("\n");
            if (analyzer.stuck() && verbosity >= DETAILED)
                analyzer.drawCandidates();
            else
                analyzer.board().draw();
            printf("\n");
        }
    }
    else
    {
        Analyzer::Step step;
        int i = 1;
        do
        {
            step = analyzer.next();
            if (verbosity == JSON)
            {
                s_json["steps"].emplace_back(step.toJson());
            }
            else
            {
                printStep(step, verbosity, i++);
                if (verbosity >= CHATTY && step.action == Analyzer::Step::ELIMINATE)
                    analyzer.drawCandidates();
            }
        } while (step.action == Analyzer::Step::ELIMINATE);

        if (verbosity == JSON)
        {
            s_json["end"] = analyzer.board().toJson();
            printf("%s\n", s_json.dump().c_str());
        }
        else
        {
            printf("\n");
            if (verbosity >= DETAILED)
                analyzer.drawCandidates();
            else if (verbosity >= VERBOSE)
                analyzer.board().draw();
        }
    }

    return 0;
}
