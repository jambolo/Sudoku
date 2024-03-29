#if !defined(BOARD_BOARD_H_INCLUDED)
#define BOARD_BOARD_H_INCLUDED 1
#pragma once

#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <vector>

// A representation of of a board and its values
class Board
{
public:
    class ForEach;
    class Group;
    class Cell;

    static int constexpr SIZE      = 9;             // Width and height of the board
    static int constexpr BOX_SIZE  = 3;             // Size of a "box"
    static int constexpr EMPTY     = 0;             // Value of an empty cell
    static int constexpr NUM_CELLS = SIZE * SIZE;   // Number of cells on a board

    // Constructs an empty board
    Board();

    // Constructs a board with initial values
    explicit Board(std::vector<int> const & v);

    // Constructs a board from a terminated string of 81 digits ('0', ' ', and '.' represent empty squares)
    explicit Board(char const * s);

    // Initializes the board
    bool initialize(std::vector<int> const & v);

    // Initializes the board from a terminated string of 81 digits ('0', ' ', and '.' represent empty squares)
    bool initialize(char const * s);

    // Returns the values of all cells in row-major order
    std::vector<int> cells() const;

    // Sets the value of a cell (1 - 9, or EMPTY)
    void set(int r, int c, int x) { board_[r][c] = x; }

    // Sets the value of a cell by index (1 - 9, or EMPTY)
    void set(int i, int x);

    // Returns the value of a cell (1 - 9, or EMPTY)
    int get(int r, int c) const { return board_[r][c]; }

    // Returns the value of a cell by index (1 - 9, or EMPTY)
    int get(int i) const;

    // Returns true if the cell is empty
    bool isEmpty(int r, int c) const { return board_[r][c] == EMPTY; }

    // Returns true if the cell by index is empty
    bool isEmpty(int i) const;

    // Returns all possible values for the cell based only on solved cells
    std::vector<int> candidates(int r, int c) const;

    // Returns all possible values for the cell by index based only on solved cells
    std::vector<int> candidates(int i) const;

    // Returns the coordinates of an empty cell at or following the given cell (in row major order), or false if there are none
    bool nextEmpty(int * nextR, int * nextC) const;

    // Returns true if the board is solved (completed and consistent)
    bool solved() const;

    // Returns true if there are no empty cells
    bool completed() const;

    // Returns true if the board is consistent (no values conflict)
    bool consistent() const;

    // Draws the board to stdout
    void draw() const;

    // Serializes the board as a string 0f '0'-'9' in row-major order
    void serialize(std::string & out) const;

    // Returns the value as a JSON object
    nlohmann::json toJson() const;

private:
    bool consistent(std::vector<int> const & group) const;
    bool boxIsConsistent(int b) const;
    bool columnIsConsistent(int c) const;
    bool rowIsConsistent(int r) const;

    static bool consistent(int x, int & values);

    int board_[SIZE][SIZE];
};

class Board::ForEach
{
public:
    // Calls a function for each cell on the board, passing the index of the cell. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool cell(std::function<bool(int)> f);

    // Calls a function for each row on the board, passing in the row number and the indexes of the cells in the row. Aborts if any
    // function call returns false.
    // Returns true if every function call returns true.
    static bool row(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the indexes of the cells
    // in the row. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool rowExcept(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the indexes of the cells
    // in the row. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool rowExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the indexes of the cells
    // in the row. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool rowExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each row on the board except the specified ones, passing in the row number and the indexes of the cells
    // in the row. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool rowExcept(int x0, int x1, int x2, int x3, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board, passing in the column number and the indexes of the cells in the column.
    // Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool column(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and the indexes of the
    // cells in the column. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool columnExcept(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and the indexes of the
    // cells in the column. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool columnExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and the indexes of the
    // cells in the column. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool columnExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each column on the board except the specified ones, passing in the column number and the indexes of the
    // cells in the column. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool columnExcept(int x0, int x1, int x2, int x3, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board, passing in the box number and the indexes of the cells in the box. Aborts if any
    // function call returns false.
    // Returns true if every function call returns true.
    static bool box(std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and the indexes of the cells
    // in the box. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool boxExcept(int x0, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and the indexes of the cells
    // in the box. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool boxExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and the indexes of the cells
    // in the box. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool boxExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f);

    // Calls a function for each box on the board except the specified ones, passing in the box number and the indexes of the cells
    // in the box. Aborts if any function call returns false.
    // Returns true if every function call returns true.
    static bool boxExcept(int x0, int x1, int x2, int x3, std::function<bool(int, std::vector<int> const &)> f);
};

class Board::Cell
{
public:
    // Returns the next cell (in row major order)
    static void next(int * r, int * c);

    // Returns indexes of all cells that depend on this one
    static std::vector<int> const & dependents(int i);

    // Returns indexes of all cells that depend on both of these
    static std::vector<int> dependents(int i0, int i1);

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

    // Returns the name of the given cell
    static std::string name(int index);

    // Returns the name of the given cell
    static std::string name(int r, int c);
};

class Board::Group
{
public:
    // Returns the indexes for the given row
    static std::vector<int> const & row(int r);

    // Returns the indexes for the given column
    static std::vector<int> const & column(int c);

    // Returns the indexes for the given box
    static std::vector<int> const & box(int b);

    // Returns the row containing the cell by index
    static int whichRow(int i) { return i / Board::SIZE; }

    // Returns the column containing the cell by index
    static int whichColumn(int i) { return i % Board::SIZE; }

    // Returns the box containing the cell by location
    static int whichBox(int r, int c) { return whichBox(Cell::indexOf(r, c)); }

    // Returns the box containing the cell by index
    static int whichBox(int i)
    {
        int by = i / Board::SIZE / 3;
        int bx = i % Board::SIZE / 3;
        return by * 3 + bx;
    }

    // Returns the offset from the beginning of the row for the index
    static int offsetInRow(int i) { return i % Board::SIZE; }

    // Returns the offset from the beginning of the column for the index
    static int offsetInColumn(int i) { return i / Board::SIZE; }

    // Returns the offset from the beginning of the box for the index
    static int offsetInBox(int i)
    {
        int r = i / Board::SIZE % 3;
        int c = i % Board::SIZE % 3;
        return r * 3 + c;
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

    // Returns the name of the given box
    static char boxName(int b)
    {
        return "123456789"[b];
    }
};

#endif // defined(BOARD_BOARD_H_INCLUDED)
