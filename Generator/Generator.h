#pragma once

#include <vector>

class Board;

class Generator
{
public:
    static Board generate(int difficulty = 0);

private:
    static Board            generateSolvedBoard();
    static bool             attempt(Board & b, int i);
    static std::vector<int> randomizedIndexes();
};
