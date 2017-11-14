#include "Board.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

Board::Board()
{
    memset(&board_, EMPTY, sizeof(board_));
}

void Board::set(int i, int x)
{
    int r, c;
    locationOf(i, &r, &c);
    set(r, c, x);
}

int Board::get(int i) const
{
    int r, c;
    locationOf(i, &r, &c);
    return get(r, c);
}

bool Board::isEmpty(int r, int c) const
{
    return board_[r][c] == EMPTY;
}

bool Board::isEmpty(int i) const
{
    int r, c;
    locationOf(i, &r, &c);
    return isEmpty(r, c);
}

std::vector<int> Board::allPossible(int r, int c) const
{
    std::vector<int> values { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> others = getDependents(r, c);
    for (int i : others)
    {
        int otherR, otherC;
        locationOf(i, &otherR, &otherC);
        int x = board_[otherR][otherC];
        values[x] = EMPTY;
    }

    // Remove EMPTYs
    values.erase(std::remove(values.begin(), values.end(), EMPTY), values.end());

    return values;
}

std::vector<int> Board::allPossible(int i) const
{
    int r, c;
    locationOf(i, &r, &c);
    return allPossible(r, c);
}

bool Board::firstEmpty(int * firstR, int * firstC) const
{
    *firstR = 0;
    *firstC = 0;
    return nextEmpty(firstR, firstC);
}

bool Board::firstEmpty(int * first) const
{
    int r, c;
    bool found = firstEmpty(&r, &c);
    if (found)
    {
        *first = indexOf(r, c);
    }
    return found;
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
    {
        return false;
    }

    *nextR = r;
    *nextC = c;
    return true;
}

bool Board::nextEmpty(int * next) const
{
    int r, c;
    locationOf(*next, &r, &c);
    bool found = nextEmpty(&r, &c);
    if (found)
    {
        *next = indexOf(r, c);
    }
    return found;
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

    for (int b = 0; b < SIZE; ++b)
    {
        if (!boxIsConsistent(b))
            return false;
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

bool Board::for_each_row(std::function<bool(std::vector<int> const &)> f) const
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (!f(getRowIndexes(r)))
        {
            return false;
        }
    }
    return true;
}

bool Board::for_each_column(std::function<bool(std::vector<int> const &)> f) const
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (!f(getColumnIndexes(c)))
        {
            return false;
        }
    }
    return true;
}

bool Board::for_each_box(std::function<bool(std::vector<int> const &)> f) const
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (!f(getBoxIndexes(b)))
        {
            return false;
        }
    }
    return true;
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
        {
            dependents.push_back(indexOf(r, j));
        }
    }

    // Add the column
    for (int i = 0; i < SIZE; ++i)
    {
        if (i != r)
        {
            dependents.push_back(indexOf(i, c));
        }
    }

    // Add the box
    int r0 = r - (r % BOX_SIZE);
    int c0 = c - (c % BOX_SIZE);

    for (int i = 0; i < BOX_SIZE; ++i)
    {
        for (int j = 0; j < BOX_SIZE; ++j)
        {
            if (r0 + i != r || c0 + j != c)
            {
                dependents.push_back(indexOf((r0 + i), (c0 + j)));
            }
        }
    }

    // Sort and remove duplicates
    std::sort(dependents.begin(), dependents.end());
    dependents.erase(std::unique(dependents.begin(), dependents.end()), dependents.end());

    return dependents;
}

std::vector<int> Board::getDependents(int i)
{
    int r, c;
    locationOf(i, &r, &c);
    return getDependents(r, c);
}

std::vector<int> Board::getRowIndexes(int r)
{
    std::vector<int> indexes;
    indexes.reserve(SIZE);
    int i = indexOf(r, 0);
    for (int c = 0; c < SIZE; ++c)
    {
        indexes.push_back(i);
        ++i;
    }
    return indexes;
}

std::vector<int> Board::getColumnIndexes(int c)
{
    std::vector<int> indexes;
    indexes.reserve(SIZE);
    int i = indexOf(0, c);
    for (int r = 0; r < SIZE; ++r)
    {
        indexes.push_back(i);
        i += SIZE;
    }
    return indexes;
}

std::vector<int> Board::getBoxIndexes(int b)
{
    int r0 = b / BOX_SIZE * BOX_SIZE;
    int c0 = b % BOX_SIZE * BOX_SIZE;
    std::vector<int> indexes;
    indexes.reserve(SIZE);
    int i0 = r0 * SIZE + c0 * BOX_SIZE;
    for (int r = 0; r < BOX_SIZE; ++r)
    {
        int i = i0;
        for (int c = 0; c < BOX_SIZE; ++c)
        {
            indexes.push_back(i);
            ++i;
        }
        i0 += SIZE;
    }
    return indexes;
}

bool Board::boxIsConsistent(int b) const
{
    int r0     = b / BOX_SIZE * BOX_SIZE;
    int c0     = b % BOX_SIZE * BOX_SIZE;
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
    for (auto const & row : board_)
    {
        if (!consistent(row[c], values))
            return false;
    }
    return true;
}

bool Board::rowIsConsistent(int r) const
{
    int values       = 0;
    auto const & row = board_[r];
    for (int c : row)
    {
        if (!consistent(c, values))
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
