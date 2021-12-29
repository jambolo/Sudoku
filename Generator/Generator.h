#if !defined(GENERATOR_GENERATOR_H_INCLUDED)
#define GENERATOR_GENERATOR_H_INCLUDED 1
#pragma once

#include <vector>

class Board;

// Generates a random board
class Generator
{
public:

    // Generates a random board with the given difficulty
    static Board generate(int difficulty = 0);

private:
    static Board            generateSolvedBoard();
    static bool             attempt(Board & b, int i);
    static std::vector<int> randomizedIndexes();
};

#endif // defined(GENERATOR_GENERATOR_H_INCLUDED)
