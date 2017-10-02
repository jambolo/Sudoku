#pragma once

#include <vector>

class Board;

class Generator
{
public:
    static Board generate(int difficulty = 0);

private:

    static bool attempt(Board & b, int r, int c);
    static std::vector<int> randomizedSquares();
};
