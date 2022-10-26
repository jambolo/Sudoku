#include "Analyzer/Hidden.h"

#include <gtest/gtest.h>

TEST(Hidden, DISABLED_Hidden)
{
}

TEST(Hidden, DISABLED_singleExists)
{
}

TEST(Hidden, DISABLED_pairExists)
{
}

TEST(Hidden, DISABLED_tripleExists)
{
}

TEST(Hidden, DISABLED_quadExists)
{
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
