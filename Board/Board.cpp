#include "Board.h"

#include <algorithm>
#include <memory>
#include <vector>

Board::Board()
{
    memset(&board_, EMPTY, sizeof(board_));
}

bool Board::isEmpty(int r, int c) const
{
    return board_[r][c] == EMPTY;
}

Board::ValueList Board::allPossible(int r, int c) const
{
    std::vector<int> values { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> others = getDependents(r, c);
    for (auto i : others)
    {
        int otherR;
        int otherC;
        locationOf(i, &otherR, &otherC);
        int x = board_[otherR][otherC];
        values[x] = 0;
    }

    // Remove EMPTYs
    values.erase(std::remove(values.begin(), values.end(), 0), values.end());

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
        increment(&r, &c);
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

    for (int r0 = 0; r0 < SIZE; r0 += BOX_SIZE)
    {
        for (int c0 = 0; c0 < SIZE; c0 += BOX_SIZE)
        {
            return boxIsConsistent(r0, c0);
        }
    }
    return true;
}

int Board::difficulty() const
{
    int difficulty = 0;
    for (int r = 0; r < SIZE; ++r)
    {
        for (int c = 0; c < SIZE; ++c)
        {
            if (isEmpty(r, c))
                ++difficulty;
        }
    }
    return difficulty;
}

void Board::draw() const
{
    printf("    1   2   3   4   5   6   7   8   9\n");
    printf("  +===+===+===+===+===+===+===+===+===+\n");
    for (int r = 0; r < SIZE; ++r)
    {
        printf("%c |", rowName(r));
        for (int c = 0; c < SIZE; ++c)
        {
            int x = board_[r][c];
            if (x > 0)
            {
                printf(" %d ", x);
            }
            else
            {
                printf("   ");
            }
            if (c % 3 == 2)
            {
                printf("|");
            }
            else
            {
                printf(":");
            }
        }
        printf("\n");
        if (r % 3 == 2)
        {
            printf("  +===+===+===+===+===+===+===+===+===+\n");
        }
        else
        {
            printf("  +---+---+---+---+---+---+---+---+---+\n");
        }
    }
}

void Board::increment(int * r, int * c)
{
    locationOf(indexOf(*r, *c) + 1, r, c);
}

std::vector<int> Board::getDependents(int r, int c)
{
    std::vector<int> dependents;
    dependents.reserve(BOX_SIZE * (SIZE - 1));

    // Add the row
    for (int j = 0; j < SIZE; ++j)
    {
        if (j != c)
            dependents.push_back(indexOf(r, j));
    }

    // Add the column
    for (int i = 0; i < SIZE; ++i)
    {
        if (i != r)
            dependents.push_back(indexOf(i, c));
    }

    // Add the box
    int r0 = r - (r % BOX_SIZE);
    int c0 = c - (c % BOX_SIZE);

    for (int i = 0; i < BOX_SIZE; ++i)
    {
        for (int j = 0; j < BOX_SIZE; ++j)
        {
            if (r0 + i != r || c0 + j != c)
                dependents.push_back(indexOf((r0 + i), (c0 + j)));
        }
    }

    return dependents;
}

std::vector<int> Board::getRowIndexes(int r)
{
    std::vector<int> indexes;
    indexes.reserve(SIZE);
    for (int c = 0; c < SIZE; ++c)
    {
        indexes.push_back(indexOf(r, c));
    }
    return indexes;
}

std::vector<int> Board::getColumnIndexes(int c)
{
    std::vector<int> indexes;
    indexes.reserve(SIZE);
    for (int r = 0; r < SIZE; ++r)
    {
        indexes.push_back(indexOf(r, c));
    }
    return indexes;
}

std::vector<int> Board::getBoxIndexes(int r0, int c0)
{
    std::vector<int> indexes;
    indexes.reserve(SIZE);
    for (int i = 0; i < BOX_SIZE; ++i)
    {
        for (int j = 0; j < BOX_SIZE; ++j)
        {
            indexes.push_back(indexOf(r0 + i, c0 + j));
        }
    }
    return indexes;
}

bool Board::boxIsConsistent(int r0, int c0) const
{
    int values = 0;
    for (int i = 0; i < BOX_SIZE; ++i)
    {
        for (int j = 0; j < BOX_SIZE; ++j)
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
