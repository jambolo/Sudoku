#include "Analyzer/Analyzer.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static Analyzer::Step const step1{ Analyzer::Step::SOLVE, Analyzer::Step::NONE, {}, {}, "" };
static Analyzer::Step const step2{ Analyzer::Step::ELIMINATE, Analyzer::Step::HIDDEN_SINGLE, { 0 }, { 1 }, "test 2" };
static Analyzer::Step const step3{ Analyzer::Step::STUCK, Analyzer::Step::HIDDEN_PAIR, { 2, 3 }, { 4, 5 }, "test 3" };
static Analyzer::Step const step4{ Analyzer::Step::DONE, Analyzer::Step::HIDDEN_TRIPLE, {}, {}, "test 4" };

static char const solved_board_string[]     = "524189637361547289879623145653498712987251364142376958238914576415762893796835421";
static char const solvable_board_string[]   = "024189637361547289879623145653498712987251364142376958238914576415762893796835421";
static char const unsolvable_board_string[] = "006700400000050070070100030800079016060301750700620004690007023037960040008000967";

static Board const empty_board;
static Board const solved_board(solved_board_string);
static Board const solvable_board(solvable_board_string);
static Board const unsolvable_board(unsolvable_board_string);

static Candidates::List unsolvable_candidates{
    0x22e, 0x126, 0x040, 0x080, 0x208, 0x10c, 0x010, 0x300, 0x326,
    0x21a, 0x114, 0x216, 0x114, 0x020, 0x15c, 0x146, 0x080, 0x306,
    0x234, 0x080, 0x234, 0x002, 0x310, 0x154, 0x164, 0x008, 0x324,
    0x100, 0x034, 0x02c, 0x030, 0x080, 0x200, 0x00c, 0x002, 0x040,
    0x214, 0x040, 0x214, 0x008, 0x110, 0x002, 0x080, 0x020, 0x304,
    0x080, 0x022, 0x22a, 0x040, 0x004, 0x120, 0x108, 0x300, 0x010,
    0x040, 0x200, 0x032, 0x120, 0x012, 0x080, 0x122, 0x004, 0x008,
    0x026, 0x008, 0x080, 0x200, 0x040, 0x124, 0x122, 0x010, 0x122,
    0x036, 0x036, 0x100, 0x034, 0x01a, 0x03c, 0x200, 0x040, 0x080
};

TEST(Analyzer_Step, techniqueName)
{
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NONE), "none");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_SINGLE), "hidden single");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_PAIR), "hidden pair");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_TRIPLE), "hidden triple");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_QUAD), "hidden quad");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_SINGLE), "naked single");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_PAIR), "naked pair");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_TRIPLE), "naked triple");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_QUAD), "naked quad");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::LOCKED_CANDIDATES), "locked candidates");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::X_WING), "x-wing");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::XY_WING), "xy-wing");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::SWORDFISH), "swordfish");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::JELLYFISH), "jellyfish");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::SIMPLE_COLORING), "simple coloring");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::UNIQUE_RECTANGLE), "unique rectangle");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::X_CYCLE), "x-cycle");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::LAST), "x-cycle");
}

TEST(Analyzer_Step, actionName)
{
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::SOLVE),     "solve");
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::ELIMINATE), "eliminate");
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::STUCK),     "stuck");
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::DONE),      "done");
}

TEST(Analyzer_Step, DISABLED_techniqueDifficulty)
{
}

TEST(Analyzer_Step, toJson)
{
    EXPECT_STREQ(step1.toJson().dump().c_str(), "{\"action\":\"solve\"}");
    EXPECT_STREQ(
        step2.toJson().dump().c_str(),
        "{\"action\":\"eliminate\",\"indexes\":[0],\"reason\":\"test 2\",\"technique\":\"hidden single\",\"values\":[1]}");
    EXPECT_STREQ(
        step3.toJson().dump().c_str(),
        "{\"action\":\"stuck\",\"indexes\":[2,3],\"reason\":\"test 3\",\"technique\":\"hidden pair\",\"values\":[4,5]}");
    EXPECT_STREQ(step4.toJson().dump().c_str(), "{\"action\":\"done\",\"reason\":\"test 4\",\"technique\":\"hidden triple\"}");
}

// Nothing to test yet
//
// TEST(Analyzer, DISABLED_Analyzer_board)
// {
//
// }

// Nothing to test yet
//
// TEST(Analyzer, DISABLED_Analyzer_board_candidates)
// {
//
// }

TEST(Analyzer, next)
{
    {
        Analyzer       analyzer(solved_board);
        Analyzer::Step step = analyzer.next();
        EXPECT_EQ(step.action, Analyzer::Step::DONE);
    }
    {
        Analyzer       analyzer(solvable_board);
        Analyzer::Step step = analyzer.next();
        EXPECT_EQ(step.action, Analyzer::Step::SOLVE);
        EXPECT_EQ(step.technique, Analyzer::Step::NAKED_SINGLE);
        EXPECT_TRUE(step.indexes.size() == 1 && step.indexes[0] == 0);
        EXPECT_TRUE(step.values.size() == 1 && step.values[0] == 5);
    }
    {
        Analyzer       analyzer(unsolvable_board, unsolvable_candidates);
        Analyzer::Step step = analyzer.next();
        EXPECT_EQ(step.action, Analyzer::Step::STUCK);
        EXPECT_EQ(step.technique, Analyzer::Step::NONE);
        EXPECT_EQ(step.indexes.size(), 0);
        EXPECT_EQ(step.values.size(), 0);
    }
}

TEST(Analyzer, board)
{
    // {
    //    Analyzer analyzer(empty_board);
    //    EXPECT_EQ(analyzer.board().cells(), empty_board.cells());
    // }
    {
        Analyzer analyzer(solved_board);
        EXPECT_EQ(analyzer.board().cells(), solved_board.cells());
    }
}

TEST(Analyzer, done)
{
    {
        Analyzer analyzer(solved_board);
        EXPECT_FALSE(analyzer.done());
        analyzer.next();
        EXPECT_TRUE(analyzer.done());
    }
    {
        Analyzer analyzer(solvable_board);
        EXPECT_FALSE(analyzer.done());
        analyzer.next();
        EXPECT_FALSE(analyzer.done());
    }
    {
        Analyzer analyzer(unsolvable_board, unsolvable_candidates);
        EXPECT_FALSE(analyzer.done());
        analyzer.next();
        EXPECT_TRUE(analyzer.done());
    }
}

TEST(Analyzer, stuck)
{
    {
        Analyzer analyzer(solved_board);
        EXPECT_FALSE(analyzer.stuck());
        analyzer.next();
        EXPECT_FALSE(analyzer.stuck());
    }
    {
        Analyzer analyzer(solvable_board);
        EXPECT_FALSE(analyzer.stuck());
        analyzer.next();
        EXPECT_FALSE(analyzer.stuck());
    }
    {
        Analyzer analyzer(unsolvable_board, unsolvable_candidates);
        EXPECT_FALSE(analyzer.stuck());
        analyzer.next();
        EXPECT_TRUE(analyzer.stuck());
    }
}

TEST(Analyzer, DISABLED_drawCandidates)
{
}

TEST(Analyzer, DISABLED_toJson)
{
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
