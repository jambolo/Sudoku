#pragma once

#include <vector>

class Board
{
public:
    Board();

    static int const SIZE  = 9;
    static int const EMPTY = 0;

    using ValueList = std::vector<int>;

    // Sets the value of a square (1 - 9, or EMPTY)
    void set(int r, int c, int x) { board_[r][c] = x; }

    // Returns the value of a square (1 - 9, or 0)
    int get(int r, int c) const { return board_[r][c]; }

    // Returns true if the square is empty
    bool isEmpty(int r, int c) const;

    // Returns all possible values for the square
    ValueList allPossible(int r, int c) const;

    // Returns true if there is an empty square, along with its coordinates
    bool firstEmpty(int * firstR, int * firstC) const;

    // Returns true if there is an empty square at or following the given square, along with its coordinates
    bool nextEmpty(int * nextR, int * nextC) const;

    // Returns true if the board is solved
    bool solved() const;

    // Returns true if there are no empty squares
    bool completed() const;

    // Returns true if the board is consistent (no values conflict)
    bool consistent() const;

    // Returns the next square
    static void increment(int &r, int &c);

private:

    bool boxIsConsistent(int r0, int c0) const;
    bool columnIsConsistent(int c) const;
    bool rowIsConsistent(int r) const;
    static bool isEmpty(int x);
    static bool consistent(int x, int & values);

    int board_[SIZE][SIZE];
};
