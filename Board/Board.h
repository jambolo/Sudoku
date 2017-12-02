#pragma once

#include <functional>
#include <vector>

class Board
{
public:
    class ForEach;
    class Unit;
    class Cell;

    Board();

    static int constexpr SIZE     = 9;
    static int constexpr BOX_SIZE = 3;
    static int constexpr EMPTY    = 0;

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
    std::vector<int> candidates(int r, int c) const;

    // Returns the coordinates of an empty square at or following the given square (in row major order), or false if there are none
    bool nextEmpty(int * nextR, int * nextC) const;

    // Returns true if the board is solved
    bool solved() const;

    // Returns true if there are no empty squares
    bool completed() const;

    // Returns true if the board is consistent (no values conflict)
    bool consistent() const;

    // Draws the board to stdout
    void draw() const;

private:
    bool        consistent(std::vector<int> const & unit) const;
    bool        boxIsConsistent(int b) const;
    bool        columnIsConsistent(int c) const;
    bool        rowIsConsistent(int r) const;

    static bool consistent(int x, int & values);

    int board_[SIZE][SIZE];
};

class Board::ForEach
{
public:
    // Calls a function for each cell on the board, passing the index of the cell. Returns true if all cells were processed
    // successfully.
    static bool cell(std::function<bool(int)> f);

    // Calls a function for each row on the board, passing in the row number and the indexes of the squares in the row. Returns true
    // if all rows were processed successfully.
    static bool row(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the
    // indexes of the squares in the row. Returns true if all rows were processed successfully.
    static bool rowExcept(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the
    // indexes of the squares in the row. Returns true if all rows were processed successfully.
    static bool rowExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the
    // indexes of the squares in the row. Returns true if all rows were processed successfully.
    static bool rowExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board, passing in the column number and the indexes of the squares in the column.
    // Returns true if all rows
    // were processed successfully.
    static bool column(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and
    // the indexes of the squares in the column. Returns true if all columns were processed successfully.
    static bool columnExcept(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and
    // the indexes of the squares in the column. Returns true if all columns were processed successfully.
    static bool columnExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and
    // the indexes of the squares in the column. Returns true if all columns were processed successfully.
    static bool columnExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board, passing in the box number and the indexes of the squares in the box. Returns true
    // if all rows were processed successfully.
    static bool box(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and
    // the indexes of the squares in the box. Returns true if all boxes were processed successfully.
    static bool boxExcept(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and
    // the indexes of the squares in the box. Returns true if all boxes were processed successfully.
    static bool boxExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and
    // the indexes of the squares in the box. Returns true if all boxes were processed successfully.
    static bool boxExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);
};

class Board::Cell
{
public:
    // Returns the next square (in row major order)
    static void next(int * r, int * c);

    // Returns indexes of all squares that depend on this one
    static std::vector<int> const & dependents(int r, int c);

    // Returns indexes of all squares that depend on this one
    static std::vector<int> const & dependents(int i);

    // Returns the index of a row and column
    static int indexOf(int r, int c)
    {
        return r * Board::SIZE + c;
    }

    // Returns the location corresponding to a given index
    static void locationOf(int index, int * r, int * c)
    {
        *r = index / Board::SIZE;
        *c = index % Board::SIZE;
    }

    // Returns the name of the given square
    static std::string name(int index);

    // Returns the name of the given square
    static std::string name(int r, int c);
};

class Board::Unit
{
public:
    // Returns the indexes for the given row
    static std::vector<int> const & row(int r);

    // Returns the indexes for the given column
    static std::vector<int> const & column(int c);

    // Returns the indexes for the given box
    static std::vector<int> const & box(int b);

    // Returns the row containing the cell
    static int whichRow(int i)
    {
        return i / Board::SIZE;
    }

    // Returns the column containing the cell
    static int whichColumn(int i)
    {
        return i % Board::SIZE;
    }

    // Returns the box containing the cell
    static int whichBox(int r, int c)
    {
        return r / Board::BOX_SIZE * Board::BOX_SIZE + c / Board::BOX_SIZE;
    }

    // Returns the box containing the cell
    static int whichBox(int i)
    {
        int r, c;
        Cell::locationOf(i, &r, &c);
        return whichBox(r, c);
    }

    // Returns the name of the given row
    static char rowName(int r)
    {
        return "ABCDEFGHJ"[r];
    }

    // Returns the name of the given column
    static char columnName(int c)
    {
        return "123456789"[c];
    }

    // Returns the name of the given column
    static char boxName(int b)
    {
        return "123456789"[b];
    }
};
