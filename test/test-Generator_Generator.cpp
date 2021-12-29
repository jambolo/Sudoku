#include "Generator/Generator.h"

#include <gtest/gtest.h>


TEST(Generator, DISABLED_generate)
{

}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
