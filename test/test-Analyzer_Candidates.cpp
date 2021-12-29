#include "Analyzer/Candidates.h"

#include <gtest/gtest.h>

TEST(Candidates, includes)
{
    for (int v = 1; v <= 9; ++v)
    {
        EXPECT_TRUE(Candidates::includes(Candidates::fromValue(v), v));
        EXPECT_FALSE(Candidates::includes((~Candidates::fromValue(v)) & Candidates::ALL, v));
    }
}

TEST(Candidates, isSolved)
{
    for (int v = 1; v <= 9; ++v)
    {
        EXPECT_TRUE(Candidates::isSolved(Candidates::fromValue(v)));
    }
    EXPECT_FALSE(Candidates::isSolved(Candidates::ALL));
}

TEST(Candidates, isBivalue)
{
    for (int a = 1; a <= 9; ++a)
    {
        for (int b = 1; b <= 9; ++b)
        {
            if (a == b)
                EXPECT_FALSE(Candidates::isBivalue(Candidates::fromValue(a)));
            else
                EXPECT_TRUE(Candidates::isBivalue(Candidates::fromValue(a)|Candidates::fromValue(b)));
        }
    }
    EXPECT_FALSE(Candidates::isBivalue(Candidates::ALL));
}

TEST(Candidates, value_fromValue)
{
    EXPECT_EQ(Candidates::value(Candidates::NONE), 0);
    for (int v = 1; v <= 9; ++v)
    {
        EXPECT_EQ(Candidates::value(Candidates::fromValue(v)), v);
    }
}

TEST(Candidates, values)
{
    EXPECT_EQ(Candidates::values(Candidates::NONE), std::vector<int>());

    for (Candidates::Type x = 0; x <= Candidates::ALL; ++x)
    {
        std::vector<int> expected;
        for (int i = 1; i <= 9; ++i)
        {
            if ((x & (1 << i)) != 0)
                expected.push_back(i);
        }
        EXPECT_EQ(Candidates::values(x), expected);
    }
}

TEST(Candidates, DISABLED_count)
{

}

TEST(Candidates, DISABLED_findAll)
{

}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
