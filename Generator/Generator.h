#pragma once

#include <vector>

class Board;

class Generator
{
public:
    static Board generate(int difficulty = 0);

private:
    static bool             attempt(Board & b, int r = 0, int c = -1);
    static std::vector<int> randomizedIndexes();
};
