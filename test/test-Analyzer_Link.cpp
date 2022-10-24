#include "Analyzer/Link.h"

#include <gtest/gtest.h>

static Link::Strong const normalized_strong1{ 1, 1, 8, 9 };
static Link::Strong const unnormalized_strong1{ 1, 1, 9, 8 };
static Link::Strong const normalized_strong2{ 1, 2, 9, 9 };
static Link::Strong const unnormalized_strong2{ 2, 1, 9, 9 };

static Link::Weak const normalized_weak{ 1, 8, 9 };
static Link::Weak const unnormalized_weak{ 1, 9, 8 };

TEST(Link_Strong, isNormalized)
{
    EXPECT_TRUE(normalized_strong1.isNormalized());
    EXPECT_FALSE(unnormalized_strong1.isNormalized());
    EXPECT_TRUE(normalized_strong2.isNormalized());
    EXPECT_FALSE(unnormalized_strong2.isNormalized());
}

TEST(Link_Strong, normalize)
{
    {
        Link::Strong link = normalized_strong1;
        link.normalize();
        EXPECT_TRUE(link.isNormalized());
    }
    {
        Link::Strong link = unnormalized_strong1;
        link.normalize();
        EXPECT_TRUE(link.isNormalized());
    }
    {
        Link::Strong link = normalized_strong2;
        link.normalize();
        EXPECT_TRUE(link.isNormalized());
    }
    {
        Link::Strong link = unnormalized_strong2;
        link.normalize();
        EXPECT_TRUE(link.isNormalized());
    }
}

TEST(Link_Strong, DISABLED_find_i)
{
}

TEST(Link_Strong, DISABLED_find_i_v)
{
}

TEST(Link_Strong, DISABLED_find_group)
{
}

TEST(Link_Strong, DISABLED_find_group_v)
{
}

TEST(Link_Strong, DISABLED_exists)
{
}

TEST(Link_Strong, DISABLED_existsIncremental)
{
}

TEST(Link_Weak, isNormalized)
{
    EXPECT_TRUE(normalized_weak.isNormalized());
    EXPECT_FALSE(unnormalized_weak.isNormalized());
}

TEST(Link_Weak, normalize)
{
    {
        Link::Weak link = normalized_weak;
        link.normalize();
        EXPECT_TRUE(link.isNormalized());
    }
    {
        Link::Weak link = unnormalized_weak;
        link.normalize();
        EXPECT_TRUE(link.isNormalized());
    }
}

TEST(Link_Weak, DISABLED_find_i)
{
}

TEST(Link_Weak, DISABLED_find_i_v)
{
}

TEST(Link_Weak, DISABLED_find_group)
{
}

TEST(Link_Weak, DISABLED_find_group_v)
{
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
