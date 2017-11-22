#pragma once

#include <functional>
#include <vector>

class Board
{
public:
    Board();

    static int const SIZE     = 9;
    static int const BOX_SIZE = 3;
    static int const EMPTY    = 0;

    // Sets the value of a square (1 - 9, or EMPTY)
    void set(int r, int c, int x) { board_[r][c] = x; }

    // Sets the value of a square by index (1 - 9, or EMPTY)
    void set(int i, int x);

    // Returns the value of a square (1 - 9, or EMPTY)
    int get(int r, int c) const { return board_[r][c]; }

    // Returns the value of a square by index (1 - 9, or EMPTY)
    int get(int i) const;

    // Returns true if the square is empty
    bool isEmpty(int r, int c) const;

    // Returns true if the square by index is empty
    bool isEmpty(int i) const;

    // Returns all possible values for the square
    std::vector<int> allPossible(int r, int c) const;

    // Returns all possible values for the square by index
    std::vector<int> allPossible(int i) const;

    // Returns the coordinates of the first empty square (in row major order), or false if there are none
    bool firstEmpty(int * firstR, int * firstC) const;

    // Returns the coordinates of the first empty square (in row major order), or false if there are none
    bool firstEmpty(int * first) const;

    // Returns the coordinates of an empty square at or following the given square (in row major order), or false if there are none
    bool nextEmpty(int * nextR, int * nextC) const;

    // Returns the coordinates of an empty square at or following the given square (in row major order), or false if there are none
    bool nextEmpty(int * next) const;

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

    // Calls a function for each row on the board, passing in the row number and the indexes of the squares in the row. Returns true if all rows were
    // processed successfully.
    static bool for_each_row(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the
    // indexes of the squares in the row. Returns true if all rows were processed successfully.
    static bool for_each_row_except(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the
    // indexes of the squares in the row. Returns true if all rows were processed successfully.
    static bool for_each_row_except(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the
    // indexes of the squares in the row. Returns true if all rows were processed successfully.
    static bool for_each_row_except(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board, passing in the column number and the indexes of the squares in the column. Returns true if all rows
    // were processed successfully.
    static bool for_each_column(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and
    // the indexes of the squares in the column. Returns true if all columns were processed successfully.
    static bool for_each_column_except(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and
    // the indexes of the squares in the column. Returns true if all columns were processed successfully.
    static bool for_each_column_except(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and
    // the indexes of the squares in the column. Returns true if all columns were processed successfully.
    static bool for_each_column_except(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board, passing in the box number and the indexes of the squares in the box. Returns true if all rows were
    // processed successfully.
    static bool for_each_box(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and
    // the indexes of the squares in the box. Returns true if all boxes were processed successfully.
    static bool for_each_box_except(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and
    // the indexes of the squares in the box. Returns true if all boxes were processed successfully.
    static bool for_each_box_except(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and
    // the indexes of the squares in the box. Returns true if all boxes were processed successfully.
    static bool for_each_box_except(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Returns the next square (in row major order)
    static void increment(int * r, int * c);

    // Returns indexes of all squares that depend on this one
    static std::vector<int> const & getDependents(int r, int c);

    // Returns indexes of all squares that depend on this one
    static std::vector<int> const & getDependents(int i);

    // Returns the index of a row and column
    static int indexOf(int r, int c) { return r * SIZE + c; }

    // Returns the location corresponding to a given index
    static void locationOf(int index, int * r, int * c) { *r = index / SIZE; *c = index % SIZE; }

    // Returns the box containing the square
    static int indexOfBox(int r, int c) { return r / BOX_SIZE * BOX_SIZE + c / BOX_SIZE; }

    // Returns the location corresponding to the upper-right corner of a given box
    static void locationOfBox(int index, int * r, int * c) { *r = (index / SIZE) / BOX_SIZE; *c = (index % SIZE) / BOX_SIZE; }

    // Returns the name of the give row
    static char rowName(int r) { return "ABCDEFGHJ"[r]; }

    // Returns the name of the given column
    static char columnName(int c) { return "123456789"[c]; }

    // Returns the name of the given column
    static char boxName(int b) { return "123456789"[b]; }

    // Returns the name of the given square
    static std::string locationName(int index);

    // Returns the name of the given square
    static std::string locationName(int r, int c);

    // Returns the indexes for the given row
    static std::vector<int> const & getRowIndexes(int r);

    // Returns the indexes for the given column
    static std::vector<int> const & getColumnIndexes(int c);

    // Returns the indexes for the given box
    static std::vector<int> const & getBoxIndexes(int b);

private:

    bool        consistent(std::vector<int> const & unit) const;
    bool        boxIsConsistent(int b) const { return consistent(getBoxIndexes(b)); }
    bool        columnIsConsistent(int c) const { return consistent(getColumnIndexes(c)); }
    bool        rowIsConsistent(int r) const { return consistent(getRowIndexes(r)); }
    static bool consistent(int x, int & values);

    int board_[SIZE][SIZE];
};
