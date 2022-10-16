#include "Board.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <algorithm>
#include <cassert>
#include <cstring>
#include <exception>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

Board::Board()
{
    memset(&board_, EMPTY, sizeof(board_));
}

Board::Board(std::vector<int> const & v)
{
    if (!initialize(v))
        throw std::invalid_argument("Board::Board: invalid argument");
}

Board::Board(char const * s)
{
    if (!initialize(s))
        throw std::invalid_argument("Board::Board: invalid argument");
}

bool Board::initialize(std::vector<int> const & v)
{
    if (v.size() != NUM_CELLS)
        return false;

    std::vector<int>::const_iterator p = v.begin();

    for (auto & row : board_)
    {
        for (auto & cell : row)
        {
            int value = *p++;
            if (value < 0 || value > 9)
                return false;
            cell = value;
        }
    }
    return true;
}

bool Board::initialize(char const * s)
{
    if (!s)
        return false;

    for (auto & row : board_)
    {
        for (auto & cell : row)
        {
            char digit = *s++;
            if (!digit || !isdigit(digit))
                return false;
            cell =  digit - '0';
        }
    }

    // Make sure there are the right number of digits
    if (*s)
        return false;

    return true;
}

std::vector<int> Board::cells() const
{
    std::vector<int> out;
    out.reserve(Board::NUM_CELLS);
    for (auto const & row : board_)
    {
        for (int cell : row)
        {
            out.push_back(cell);
        }
    }
    return out;
}

void Board::serialize(std::string & out) const
{
    out.clear();
    out.reserve(Board::NUM_CELLS);
    for (auto const & row : board_)
    {
        for (int cell : row)
        {
            out.push_back(cell + '0');
        }
    }
}

void Board::set(int i, int x)
{
    int r, c;
    Cell::locationOf(i, &r, &c);
    set(r, c, x);
}

int Board::get(int i) const
{
    int r, c;
    Cell::locationOf(i, &r, &c);
    return get(r, c);
}

bool Board::isEmpty(int i) const
{
    int r, c;
    Cell::locationOf(i, &r, &c);
    return isEmpty(r, c);
}

std::vector<int> Board::candidates(int r, int c) const
{
    return candidates(Cell::indexOf(r, c));
}

std::vector<int> Board::candidates(int i) const
{
    std::vector<int> values{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> dependents = Cell::dependents(i);
    for (int d : dependents)
    {
        int v = get(d);
        values[v] = (int)EMPTY; // Remove the dependent cell's value from the list of candidates
    }

    // Remove EMPTYs, leaving just the candidates
    values.erase(std::remove(values.begin(), values.end(), (int)EMPTY), values.end());

    return values;
}

bool Board::nextEmpty(int * nextR, int * nextC) const
{
    int r = *nextR;
    int c = *nextC;
    while (r < SIZE && !isEmpty(r, c))
    {
        Cell::next(&r, &c);
    }
    if (r >= SIZE)
        return false;

    *nextR = r;
    *nextC = c;
    return true;
}

bool Board::solved() const
{
    return completed() && consistent();
}

bool Board::completed() const
{
    int r = 0;
    int c = 0;
    return !nextEmpty(&r, &c);
}

bool Board::consistent() const
{
    for (int i = 0; i < SIZE; ++i)
    {
        if (!rowIsConsistent(i) || !columnIsConsistent(i) || !boxIsConsistent(i))
            return false;
    }
    return true;
}

bool Board::boxIsConsistent(int b) const
{
    return consistent(Group::box(b));
}

bool Board::columnIsConsistent(int c) const
{
    return consistent(Group::column(c));
}

bool Board::rowIsConsistent(int r) const
{
    return consistent(Group::row(r));
}

void Board::draw() const
{
    printf("    1   2   3   4   5   6   7   8   9\n");
    printf("  +===+===+===+===+===+===+===+===+===+\n");
    for (int r = 0; r < SIZE; ++r)
    {
        printf("%c |", Group::rowName(r));
        for (int c = 0; c < SIZE; ++c)
        {
            int x = board_[r][c];
            if (x > 0)
                printf(" %d ", x);
            else
                printf("   ");
            if (c % 3 == 2)
                printf("|");
            else
                printf(":");
        }
        printf("\n");
        if (r % 3 == 2)
            printf("  +===+===+===+===+===+===+===+===+===+\n");
        else
            printf("  +---+---+---+---+---+---+---+---+---+\n");
    }
}

nlohmann::json Board::toJson() const
{
    return json(board_);
}

bool Board::ForEach::cell(std::function<bool(int)> f)
{
    for (int i = 0; i < NUM_CELLS; ++i)
    {
        if (!f(i))
            return false;
    }
    return true;
}

bool Board::ForEach::row(std::function<bool(int, std::vector<int> const &)> f)
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (!f(r, Group::row(r)))
            return false;
    }
    return true;
}

bool Board::ForEach::rowExcept(int x0, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (r != x0)
        {
            if (!f(r, Group::row(r)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::rowExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (r != x0 && r != x1)
        {
            if (!f(r, Group::row(r)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::rowExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (r != x0 && r != x1 && r != x2)
        {
            if (!f(r, Group::row(r)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::rowExcept(int x0, int x1, int x2, int x3, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (r != x0 && r != x1 && r != x2 && r != x3)
        {
            if (!f(r, Group::row(r)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::column(std::function<bool(int, std::vector<int> const &)> f)
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (!f(c, Group::column(c)))
            return false;
    }
    return true;
}

bool Board::ForEach::columnExcept(int x0, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (c != x0)
        {
            if (!f(c, Group::column(c)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::columnExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (c != x0 && c != x1)
        {
            if (!f(c, Group::column(c)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::columnExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (c != x0 && c != x1 && c != x2)
        {
            if (!f(c, Group::column(c)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::columnExcept(int x0, int x1, int x2, int x3, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (c != x0 && c != x1 && c != x2 && c != x3)
        {
            if (!f(c, Group::column(c)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::box(std::function<bool(int, std::vector<int> const &)> f)
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (!f(b, Group::box(b)))
            return false;
    }
    return true;
}

bool Board::ForEach::boxExcept(int x0, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (b != x0)
        {
            if (!f(b, Group::box(b)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::boxExcept(int x0, int x1, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (b != x0 && b != x1)
        {
            if (!f(b, Group::box(b)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::boxExcept(int x0, int x1, int x2, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (b != x0 && b != x1 && b != x2)
        {
            if (!f(b, Group::box(b)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::boxExcept(int x0, int x1, int x2, int x3, std::function<bool(int, std::vector<int> const &)> f)
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (b != x0 && b != x1 && b != x2 && b != x3)
        {
            if (!f(b, Group::box(b)))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::indexExcept(std::vector<int> const & indexes, int x0, std::function<bool(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0)
        {
            if (!f(i))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::indexExcept(std::vector<int> const & indexes, int x0, int x1, std::function<bool(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0 && i != x1)
        {
            if (!f(i))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::indexExcept(std::vector<int> const & indexes, int x0, int x1, int x2, std::function<bool(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0 && i != x1 && i != x2)
        {
            if (!f(i))
                return false;
        }
    }
    return true;
}

bool Board::ForEach::indexExcept(std::vector<int> const & indexes, int x0, int x1, int x2, int x3, std::function<bool(int)> f)
{
    for (int i : indexes)
    {
        if (i != x0 && i != x1 && i != x2 && i != x3)
        {
            if (!f(i))
                return false;
        }
    }
    return true;
}

void Board::Cell::next(int * r, int * c)
{
    Cell::locationOf(Cell::indexOf(*r, *c) + 1, r, c);
}

std::vector<int> const & Board::Cell::dependents(int i)
{
    static std::vector<int> const DEPENDENTS_BY_INDEX[NUM_CELLS] =
    {
        {   1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
        {   0, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 18, 19, 20, 28, 37, 46, 55, 64, 73 },
        {   0, 1,  3,  4,  5,  6,  7,  8,  9,  10, 11, 18, 19, 20, 29, 38, 47, 56, 65, 74 },
        {   0, 1,  2,  4,  5,  6,  7,  8,  12, 13, 14, 21, 22, 23, 30, 39, 48, 57, 66, 75 },
        {   0, 1,  2,  3,  5,  6,  7,  8,  12, 13, 14, 21, 22, 23, 31, 40, 49, 58, 67, 76 },
        {   0, 1,  2,  3,  4,  6,  7,  8,  12, 13, 14, 21, 22, 23, 32, 41, 50, 59, 68, 77 },
        {   0, 1,  2,  3,  4,  5,  7,  8,  15, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 },
        {   0, 1,  2,  3,  4,  5,  6,  8,  15, 16, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 },
        {   0, 1,  2,  3,  4,  5,  6,  7,  15, 16, 17, 24, 25, 26, 35, 44, 53, 62, 71, 80 },
        {   0, 1,  2,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
        {   0, 1,  2,  9,  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 28, 37, 46, 55, 64, 73 },
        {   0, 1,  2,  9,  10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 29, 38, 47, 56, 65, 74 },
        {   3, 4,  5,  9,  10, 11, 13, 14, 15, 16, 17, 21, 22, 23, 30, 39, 48, 57, 66, 75 },
        {   3, 4,  5,  9,  10, 11, 12, 14, 15, 16, 17, 21, 22, 23, 31, 40, 49, 58, 67, 76 },
        {   3, 4,  5,  9,  10, 11, 12, 13, 15, 16, 17, 21, 22, 23, 32, 41, 50, 59, 68, 77 },
        {   6, 7,  8,  9,  10, 11, 12, 13, 14, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 },
        {   6, 7,  8,  9,  10, 11, 12, 13, 14, 15, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 },
        {   6, 7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 24, 25, 26, 35, 44, 53, 62, 71, 80 },
        {   0, 1,  2,  9,  10, 11, 19, 20, 21, 22, 23, 24, 25, 26, 27, 36, 45, 54, 63, 72 },
        {   0, 1,  2,  9,  10, 11, 18, 20, 21, 22, 23, 24, 25, 26, 28, 37, 46, 55, 64, 73 },
        {   0, 1,  2,  9,  10, 11, 18, 19, 21, 22, 23, 24, 25, 26, 29, 38, 47, 56, 65, 74 },
        {   3, 4,  5,  12, 13, 14, 18, 19, 20, 22, 23, 24, 25, 26, 30, 39, 48, 57, 66, 75 },
        {   3, 4,  5,  12, 13, 14, 18, 19, 20, 21, 23, 24, 25, 26, 31, 40, 49, 58, 67, 76 },
        {   3, 4,  5,  12, 13, 14, 18, 19, 20, 21, 22, 24, 25, 26, 32, 41, 50, 59, 68, 77 },
        {   6, 7,  8,  15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 33, 42, 51, 60, 69, 78 },
        {   6, 7,  8,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 34, 43, 52, 61, 70, 79 },
        {   6, 7,  8,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 35, 44, 53, 62, 71, 80 },
        {   0, 9,  18, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 54, 63, 72 },
        {   1, 10, 19, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 55, 64, 73 },
        {   2, 11, 20, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 56, 65, 74 },
        {   3, 12, 21, 27, 28, 29, 31, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 57, 66, 75 },
        {   4, 13, 22, 27, 28, 29, 30, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 58, 67, 76 },
        {   5, 14, 23, 27, 28, 29, 30, 31, 33, 34, 35, 39, 40, 41, 48, 49, 50, 59, 68, 77 },
        {   6, 15, 24, 27, 28, 29, 30, 31, 32, 34, 35, 42, 43, 44, 51, 52, 53, 60, 69, 78 },
        {   7, 16, 25, 27, 28, 29, 30, 31, 32, 33, 35, 42, 43, 44, 51, 52, 53, 61, 70, 79 },
        {   8, 17, 26, 27, 28, 29, 30, 31, 32, 33, 34, 42, 43, 44, 51, 52, 53, 62, 71, 80 },
        {   0, 9,  18, 27, 28, 29, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 54, 63, 72 },
        {   1, 10, 19, 27, 28, 29, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 55, 64, 73 },
        {   2, 11, 20, 27, 28, 29, 36, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 56, 65, 74 },
        {   3, 12, 21, 30, 31, 32, 36, 37, 38, 40, 41, 42, 43, 44, 48, 49, 50, 57, 66, 75 },
        {   4, 13, 22, 30, 31, 32, 36, 37, 38, 39, 41, 42, 43, 44, 48, 49, 50, 58, 67, 76 },
        {   5, 14, 23, 30, 31, 32, 36, 37, 38, 39, 40, 42, 43, 44, 48, 49, 50, 59, 68, 77 },
        {   6, 15, 24, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 51, 52, 53, 60, 69, 78 },
        {   7, 16, 25, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 44, 51, 52, 53, 61, 70, 79 },
        {   8, 17, 26, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 51, 52, 53, 62, 71, 80 },
        {   0, 9,  18, 27, 28, 29, 36, 37, 38, 46, 47, 48, 49, 50, 51, 52, 53, 54, 63, 72 },
        {   1, 10, 19, 27, 28, 29, 36, 37, 38, 45, 47, 48, 49, 50, 51, 52, 53, 55, 64, 73 },
        {   2, 11, 20, 27, 28, 29, 36, 37, 38, 45, 46, 48, 49, 50, 51, 52, 53, 56, 65, 74 },
        {   3, 12, 21, 30, 31, 32, 39, 40, 41, 45, 46, 47, 49, 50, 51, 52, 53, 57, 66, 75 },
        {   4, 13, 22, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 50, 51, 52, 53, 58, 67, 76 },
        {   5, 14, 23, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 49, 51, 52, 53, 59, 68, 77 },
        {   6, 15, 24, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 60, 69, 78 },
        {   7, 16, 25, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 53, 61, 70, 79 },
        {   8, 17, 26, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 62, 71, 80 },
        {   0, 9,  18, 27, 36, 45, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
        {   1, 10, 19, 28, 37, 46, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
        {   2, 11, 20, 29, 38, 47, 54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
        {   3, 12, 21, 30, 39, 48, 54, 55, 56, 58, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
        {   4, 13, 22, 31, 40, 49, 54, 55, 56, 57, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
        {   5, 14, 23, 32, 41, 50, 54, 55, 56, 57, 58, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
        {   6, 15, 24, 33, 42, 51, 54, 55, 56, 57, 58, 59, 61, 62, 69, 70, 71, 78, 79, 80 },
        {   7, 16, 25, 34, 43, 52, 54, 55, 56, 57, 58, 59, 60, 62, 69, 70, 71, 78, 79, 80 },
        {   8, 17, 26, 35, 44, 53, 54, 55, 56, 57, 58, 59, 60, 61, 69, 70, 71, 78, 79, 80 },
        {   0, 9,  18, 27, 36, 45, 54, 55, 56, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
        {   1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
        {   2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
        {   3, 12, 21, 30, 39, 48, 57, 58, 59, 63, 64, 65, 67, 68, 69, 70, 71, 75, 76, 77 },
        {   4, 13, 22, 31, 40, 49, 57, 58, 59, 63, 64, 65, 66, 68, 69, 70, 71, 75, 76, 77 },
        {   5, 14, 23, 32, 41, 50, 57, 58, 59, 63, 64, 65, 66, 67, 69, 70, 71, 75, 76, 77 },
        {   6, 15, 24, 33, 42, 51, 60, 61, 62, 63, 64, 65, 66, 67, 68, 70, 71, 78, 79, 80 },
        {   7, 16, 25, 34, 43, 52, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 78, 79, 80 },
        {   8, 17, 26, 35, 44, 53, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 78, 79, 80 },
        {   0, 9,  18, 27, 36, 45, 54, 55, 56, 63, 64, 65, 73, 74, 75, 76, 77, 78, 79, 80 },
        {   1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 64, 65, 72, 74, 75, 76, 77, 78, 79, 80 },
        {   2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 65, 72, 73, 75, 76, 77, 78, 79, 80 },
        {   3, 12, 21, 30, 39, 48, 57, 58, 59, 66, 67, 68, 72, 73, 74, 76, 77, 78, 79, 80 },
        {   4, 13, 22, 31, 40, 49, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 77, 78, 79, 80 },
        {   5, 14, 23, 32, 41, 50, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 76, 78, 79, 80 },
        {   6, 15, 24, 33, 42, 51, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 79, 80 },
        {   7, 16, 25, 34, 43, 52, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80 },
        {   8, 17, 26, 35, 44, 53, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 }
    };
    return DEPENDENTS_BY_INDEX[i];
}

std::vector<int> Board::Cell::dependents(int i0, int i1)
{
    std::vector<int> const & dependents0 = Board::Cell::dependents(i0);
    std::vector<int> const & dependents1 = Board::Cell::dependents(i1);
    std::vector<int>         intersection;
    std::set_intersection(dependents0.begin(), dependents0.end(),
                          dependents1.begin(), dependents1.end(),
                          std::back_inserter(intersection));
    return intersection;
}

std::string Board::Cell::name(int index)
{
    int r, c;
    Cell::locationOf(index, &r, &c);
    return Cell::name(r, c);
}

std::string Board::Cell::name(int r, int c)
{
    return std::string(1, Group::rowName(r)) + Group::columnName(c);
}

std::vector<int> const & Board::Group::row(int r)
{
    static std::vector<int> const INDEXES_BY_ROW[SIZE] =
    {
        {  0, 1,  2,  3,  4,  5,  6,  7,  8  },
        {  9, 10, 11, 12, 13, 14, 15, 16, 17 },
        { 18, 19, 20, 21, 22, 23, 24, 25, 26 },
        { 27, 28, 29, 30, 31, 32, 33, 34, 35 },
        { 36, 37, 38, 39, 40, 41, 42, 43, 44 },
        { 45, 46, 47, 48, 49, 50, 51, 52, 53 },
        { 54, 55, 56, 57, 58, 59, 60, 61, 62 },
        { 63, 64, 65, 66, 67, 68, 69, 70, 71 },
        { 72, 73, 74, 75, 76, 77, 78, 79, 80 }
    };

    return INDEXES_BY_ROW[r];
}

std::vector<int> const & Board::Group::column(int c)
{
    static std::vector<int> const INDEXES_BY_COLUMN[SIZE] =
    {
        {  0, 9,  18, 27, 36, 45, 54, 63, 72 },
        {  1, 10, 19, 28, 37, 46, 55, 64, 73 },
        {  2, 11, 20, 29, 38, 47, 56, 65, 74 },
        {  3, 12, 21, 30, 39, 48, 57, 66, 75 },
        {  4, 13, 22, 31, 40, 49, 58, 67, 76 },
        {  5, 14, 23, 32, 41, 50, 59, 68, 77 },
        {  6, 15, 24, 33, 42, 51, 60, 69, 78 },
        {  7, 16, 25, 34, 43, 52, 61, 70, 79 },
        {  8, 17, 26, 35, 44, 53, 62, 71, 80 }
    };

    return INDEXES_BY_COLUMN[c];
}

std::vector<int> const & Board::Group::box(int b)
{
    static std::vector<int> const INDEXES_BY_BOX[SIZE] =
    {
        {  0, 1,  2,  9,  10, 11, 18, 19, 20 },
        {  3, 4,  5,  12, 13, 14, 21, 22, 23 },
        {  6, 7,  8,  15, 16, 17, 24, 25, 26 },
        { 27, 28, 29, 36, 37, 38, 45, 46, 47 },
        { 30, 31, 32, 39, 40, 41, 48, 49, 50 },
        { 33, 34, 35, 42, 43, 44, 51, 52, 53 },
        { 54, 55, 56, 63, 64, 65, 72, 73, 74 },
        { 57, 58, 59, 66, 67, 68, 75, 76, 77 },
        { 60, 61, 62, 69, 70, 71, 78, 79, 80 }
    };

    return INDEXES_BY_BOX[b];
}

bool Board::consistent(std::vector<int> const & group) const
{
    int values = 0;
    for (int i : group)
    {
        if (!consistent(get(i), values))
            return false;
    }
    return true;
}

bool Board::consistent(int x, int & values)
{
    if (x == EMPTY)
        return true;

    int mask = 1 << x;
    if ((values & mask) != 0)
        return false;

    values |= mask;
    return true;
}
