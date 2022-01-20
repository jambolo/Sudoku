#include "Analyzer/Analyzer.h"

#include <gtest/gtest.h>

TEST(Analyzer_Step, techniqueName)
{
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NONE                ), "none"             );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_SINGLE       ), "hidden single"    );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_PAIR         ), "hidden pair"      );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_TRIPLE       ), "hidden triple"    );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::HIDDEN_QUAD         ), "hidden quad"      );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_SINGLE        ), "naked single"     );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_PAIR          ), "naked pair"       );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_TRIPLE        ), "naked triple"     );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::NAKED_QUAD          ), "naked quad"       );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::LOCKED_CANDIDATES   ), "locked candidates");
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::X_WING              ), "x-wing"           );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::XY_WING             ), "xy-wing"          );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::SWORDFISH           ), "swordfish"        );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::JELLYFISH           ), "jellyfish"        );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::SIMPLE_COLORING     ), "simple coloring"  );
    EXPECT_STREQ(Analyzer::Step::techniqueName(Analyzer::Step::LAST                ), "simple coloring"  );
}

TEST(Analyzer_Step, actionName)
{
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::SOLVE),     "solve"    );
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::ELIMINATE), "eliminate");
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::STUCK),     "stuck"    );
    EXPECT_STREQ(Analyzer::Step::actionName(Analyzer::Step::DONE),      "done"     );

}

TEST(Analyzer_Step, DISABLED_techniqueDifficulty)
{

}

TEST(Analyzer_Step, DISABLED_toJson)
{

}

// Nothing to test yet
//
//TEST(Analyzer, DISABLED_Analyzer_board)
//{
//
//}

// Nothing to test yet
//
//TEST(Analyzer, DISABLED_Analyzer_board_candidates)
//{
//
//}

TEST(Analyzer, DISABLED_next)
{

}

TEST(Analyzer, DISABLED_board)
{

}

TEST(Analyzer, DISABLED_done)
{

}

TEST(Analyzer, DISABLED_stuck)
{

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
