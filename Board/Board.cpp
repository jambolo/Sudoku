#include "Board.h"

#include <algorithm>
#include <memory>

Board::Board()
{
    memset(&board_, EMPTY, sizeof(board_));
}

bool Board::isEmpty(int r, int c) const
{
    return board_[r][c] == EMPTY;
}

bool Board::isPossible(int r, int c, int x) const
{
    return !rowContains(r, x) && !columnContains(c, x) && !boxContains(r, c, x);
}

Board::ValueList Board::allPossible(int r, int c) const
{
    ValueList values = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    // Check the row
    for (int j = 0; j < SIZE; ++j)
    {
        int x = board_[r][j];
        if (!isEmpty(x))
            values[x] = EMPTY;
    }

    // Check the column
    for (int i = 0; i < SIZE; ++i)
    {
        int x = board_[i][c];
        if (!isEmpty(x))
            values[x] = EMPTY;
    }

    // Check the box
    int r0 = r - (r % 3);
    int c0 = c - (c % 3);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int x = board_[r0 + i][c0 + j];
            values[x] = EMPTY;
        }
    }

    // Remove EMPTYs
    values.erase(std::remove(values.begin(), values.end(), EMPTY), values.end());

    return values;
}

bool Board::firstEmpty(int * firstR, int * firstC) const
{
    *firstR = 0;
    *firstC = 0;
    return nextEmpty(firstR, firstC);
}

bool Board::nextEmpty(int * nextR, int * nextC) const
{
    int r = *nextR;
    int c = *nextC;
    while (r < SIZE && !isEmpty(r, c))
    {
        increment(r, c);
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
    int r, c;
    return !firstEmpty(&r, &c);
}

bool Board::consistent() const
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (!rowIsConsistent(r))
            return false;
    }

    for (int c = 0; c < SIZE; ++c)
    {
        if (!columnIsConsistent(c))
            return false;
    }

    for (int r0 = 0; r0 < SIZE; r0 += 3)
    {
        for (int c0 = 0; c0 < SIZE; c0 += 3)
        {
            return boxIsConsistent(r0, c0);
        }
    }
    return true;
}

void Board::increment(int & r, int & c)
{
    ++c;
    if (c >= SIZE)
    {
        ++r;
        c = 0;
    }
}

bool Board::boxContains(int r, int c, int x) const
{
    int r0 = r - (r % 3);
    int c0 = c - (c % 3);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (board_[c0 + i][r0 + j] == x)
                return true;
        }
    }
    return false;
}

bool Board::columnContains(int c, int x) const
{
    for (int i = 0; i < 9; ++i)
    {
        if (board_[i][c] == x)
            return true;
    }
    return false;
}

bool Board::rowContains(int r, int x) const
{
    for (int j = 0; j < 9; ++j)
    {
        if (board_[r][j] == x)
            return true;
    }
    return false;
}

bool Board::boxIsConsistent(int r0, int c0) const
{
    int values = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (!consistent(board_[r0 + i][c0 + j], values))
                return false;
        }
    }
    return true;
}

bool Board::columnIsConsistent(int c) const
{
    int values = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        if (!consistent(board_[i][c], values))
            return false;
    }
    return true;
}

bool Board::rowIsConsistent(int r) const
{
    int values = 0;
    for (int j = 0; j < SIZE; ++j)
    {
        if (!consistent(board_[r][j], values))
            return false;
    }
    return true;
}

bool Board::isEmpty(int x)
{
    return x == EMPTY;
}

bool Board::consistent(int x, int & values)
{
    if (isEmpty(x))
        return true;

    int mask = 1 << x;
    if ((values & mask) != 0)
        return false;

    values |= mask;
    return true;
}

