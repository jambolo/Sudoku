#pragma once

#include <vector>

class Board
{
public:
    Board();

    static int const SIZE     = 9;
    static int const BOX_SIZE = 3;
    static int const EMPTY    = 0;

    using ValueList = std::vector<int>;

    // Sets the value of a square (1 - 9, or EMPTY)
    void set(int r, int c, int x) { board_[r][c] = x; }

    // Sets the value of a square by index (1 - 9, or EMPTY)
    void set(int i, int x) { int r, c; locationOf(i, &r, &c); set(r, c, x); }

    // Returns the value of a square (1 - 9, or EMPTY)
    int get(int r, int c) const { return board_[r][c]; }

    // Returns the value of a square by index (1 - 9, or EMPTY)
    int get(int i) const { int r, c; locationOf(i, &r, &c); return get(r, c); }

    // Returns true if the square is empty
    bool isEmpty(int r, int c) const;

    // Returns true if the square by index is empty
    bool isEmpty(int i) const { int r, c; locationOf(i, &r, &c); return isEmpty(r, c); }

    // Returns all possible values for the square
    ValueList allPossible(int r, int c) const;

    // Returns all possible values for the square by index
    ValueList allPossible(int i) const { int r, c; locationOf(i, &r, &c); return allPossible(r, c); }

    // Returns the coordinates of an empty square, or false if there are none
    bool firstEmpty(int * firstR, int * firstC) const;

    // Returns the coordinates of an empty square at or following the given square (in row major order), or false if there are none
    bool nextEmpty(int * nextR, int * nextC) const;

    // Returns true if the board is solved
    bool solved() const;

    // Returns true if there are no empty squares
    bool completed() const;

    // Returns true if the board is consistent (no values conflict)
    bool consistent() const;

    // Returns a measure of the difficulty in the range [0, 81]
    int difficulty() const;

    // Draws the board to stdout
    void draw() const;

    // Returns the next square (in row major order)
    static void increment(int * r, int * c);

    // Returns indexes of all squares that depend on this one
    static std::vector<int> getDependents(int r, int c);

    // Returns the index of a row and column
    static int indexOf(int r, int c) { return r * SIZE + c; }

    // Returns the location corresponding to a given index
    static void locationOf(int index, int * r, int * c) { *r = index / SIZE; *c = index % SIZE; }

    // Returns the name of the give row
    static char rowName(int r)    { return "ABCDEFGHJ"[r]; }

    // Returns the name of the given column
    static char columnName(int c) { return "123456789"[c]; }

    // Returns the indexes for the given row
    static std::vector<int> getRowIndexes(int r);

    // Returns the indexes for the given column
    static std::vector<int> getColumnIndexes(int c);

    // Returns the indexes for the box at the given location
    static std::vector<int> getBoxIndexes(int r, int c);

private:

    bool        boxIsConsistent(int r0, int c0) const;
    bool        columnIsConsistent(int c) const;
    bool        rowIsConsistent(int r) const;
    static bool consistent(int x, int & values);

    int board_[SIZE][SIZE];
};
