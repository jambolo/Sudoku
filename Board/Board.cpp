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
        *first = indexOf(r, c);
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
        return false;

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
        *next = indexOf(r, c);
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

bool Board::for_each_row(std::function<bool(int r, std::vector<int> const &)> f) const
{
    for (int r = 0; r < SIZE; ++r)
    {
        if (!f(r, getRowIndexes(r)))
            return false;
    }
    return true;
}

bool Board::for_each_column(std::function<bool(int c, std::vector<int> const &)> f) const
{
    for (int c = 0; c < SIZE; ++c)
    {
        if (!f(c, getColumnIndexes(c)))
            return false;
    }
    return true;
}

bool Board::for_each_box(std::function<bool(int b, std::vector<int> const &)> f) const
{
    for (int b = 0; b < SIZE; ++b)
    {
        if (!f(b, getBoxIndexes(b)))
            return false;
    }
    return true;
}

void Board::increment(int * r, int * c)
{
    locationOf(indexOf(*r, *c) + 1, r, c);
}

std::vector<int> const & Board::getDependents(int r, int c)
{
    return getDependents(indexOf(r, c));
}

std::vector<int> const & Board::getDependents(int i)
{
    static std::vector<int> const DEPENDENTS_BY_INDEX[SIZE * SIZE] =
    {
        {  1, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
        {  0, 2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 18, 19, 20, 28, 37, 46, 55, 64, 73 },
        {  0, 1,  3,  4,  5,  6,  7,  8,  9,  10, 11, 18, 19, 20, 29, 38, 47, 56, 65, 74 },
        {  0, 1,  2,  4,  5,  6,  7,  8,  12, 13, 14, 21, 22, 23, 30, 39, 48, 57, 66, 75 },
        {  0, 1,  2,  3,  5,  6,  7,  8,  12, 13, 14, 21, 22, 23, 31, 40, 49, 58, 67, 76 },
        {  0, 1,  2,  3,  4,  6,  7,  8,  12, 13, 14, 21, 22, 23, 32, 41, 50, 59, 68, 77 },
        {  0, 1,  2,  3,  4,  5,  7,  8,  15, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 },
        {  0, 1,  2,  3,  4,  5,  6,  8,  15, 16, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 },
        {  0, 1,  2,  3,  4,  5,  6,  7,  15, 16, 17, 24, 25, 26, 35, 44, 53, 62, 71, 80 },
        {  0, 1,  2,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
        {  0, 1,  2,  9,  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 28, 37, 46, 55, 64, 73 },
        {  0, 1,  2,  9,  10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 29, 38, 47, 56, 65, 74 },
        {  3, 4,  5,  9,  10, 11, 13, 14, 15, 16, 17, 21, 22, 23, 30, 39, 48, 57, 66, 75 },
        {  3, 4,  5,  9,  10, 11, 12, 14, 15, 16, 17, 21, 22, 23, 31, 40, 49, 58, 67, 76 },
        {  3, 4,  5,  9,  10, 11, 12, 13, 15, 16, 17, 21, 22, 23, 32, 41, 50, 59, 68, 77 },
        {  6, 7,  8,  9,  10, 11, 12, 13, 14, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 },
        {  6, 7,  8,  9,  10, 11, 12, 13, 14, 15, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 },
        {  6, 7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 24, 25, 26, 35, 44, 53, 62, 71, 80 },
        {  0, 1,  2,  9,  10, 11, 19, 20, 21, 22, 23, 24, 25, 26, 27, 36, 45, 54, 63, 72 },
        {  0, 1,  2,  9,  10, 11, 18, 20, 21, 22, 23, 24, 25, 26, 28, 37, 46, 55, 64, 73 },
        {  0, 1,  2,  9,  10, 11, 18, 19, 21, 22, 23, 24, 25, 26, 29, 38, 47, 56, 65, 74 },
        {  3, 4,  5,  12, 13, 14, 18, 19, 20, 22, 23, 24, 25, 26, 30, 39, 48, 57, 66, 75 },
        {  3, 4,  5,  12, 13, 14, 18, 19, 20, 21, 23, 24, 25, 26, 31, 40, 49, 58, 67, 76 },
        {  3, 4,  5,  12, 13, 14, 18, 19, 20, 21, 22, 24, 25, 26, 32, 41, 50, 59, 68, 77 },
        {  6, 7,  8,  15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 33, 42, 51, 60, 69, 78 },
        {  6, 7,  8,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 34, 43, 52, 61, 70, 79 },
        {  6, 7,  8,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 35, 44, 53, 62, 71, 80 },
        {  0, 9,  18, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 54, 63, 72 },
        {  1, 10, 19, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 55, 64, 73 },
        {  2, 11, 20, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 56, 65, 74 },
        {  3, 12, 21, 27, 28, 29, 31, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 57, 66, 75 },
        {  4, 13, 22, 27, 28, 29, 30, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 58, 67, 76 },
        {  5, 14, 23, 27, 28, 29, 30, 31, 33, 34, 35, 39, 40, 41, 48, 49, 50, 59, 68, 77 },
        {  6, 15, 24, 27, 28, 29, 30, 31, 32, 34, 35, 42, 43, 44, 51, 52, 53, 60, 69, 78 },
        {  7, 16, 25, 27, 28, 29, 30, 31, 32, 33, 35, 42, 43, 44, 51, 52, 53, 61, 70, 79 },
        {  8, 17, 26, 27, 28, 29, 30, 31, 32, 33, 34, 42, 43, 44, 51, 52, 53, 62, 71, 80 },
        {  0, 9,  18, 27, 28, 29, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 54, 63, 72 },
        {  1, 10, 19, 27, 28, 29, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 55, 64, 73 },
        {  2, 11, 20, 27, 28, 29, 36, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 56, 65, 74 },
        {  3, 12, 21, 30, 31, 32, 36, 37, 38, 40, 41, 42, 43, 44, 48, 49, 50, 57, 66, 75 },
        {  4, 13, 22, 30, 31, 32, 36, 37, 38, 39, 41, 42, 43, 44, 48, 49, 50, 58, 67, 76 },
        {  5, 14, 23, 30, 31, 32, 36, 37, 38, 39, 40, 42, 43, 44, 48, 49, 50, 59, 68, 77 },
        {  6, 15, 24, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 51, 52, 53, 60, 69, 78 },
        {  7, 16, 25, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 44, 51, 52, 53, 61, 70, 79 },
        {  8, 17, 26, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 51, 52, 53, 62, 71, 80 },
        {  0, 9,  18, 27, 28, 29, 36, 37, 38, 46, 47, 48, 49, 50, 51, 52, 53, 54, 63, 72 },
        {  1, 10, 19, 27, 28, 29, 36, 37, 38, 45, 47, 48, 49, 50, 51, 52, 53, 55, 64, 73 },
        {  2, 11, 20, 27, 28, 29, 36, 37, 38, 45, 46, 48, 49, 50, 51, 52, 53, 56, 65, 74 },
        {  3, 12, 21, 30, 31, 32, 39, 40, 41, 45, 46, 47, 49, 50, 51, 52, 53, 57, 66, 75 },
        {  4, 13, 22, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 50, 51, 52, 53, 58, 67, 76 },
        {  5, 14, 23, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 49, 51, 52, 53, 59, 68, 77 },
        {  6, 15, 24, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 60, 69, 78 },
        {  7, 16, 25, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 53, 61, 70, 79 },
        {  8, 17, 26, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 62, 71, 80 },
        {  0, 9,  18, 27, 36, 45, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
        {  1, 10, 19, 28, 37, 46, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
        {  2, 11, 20, 29, 38, 47, 54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
        {  3, 12, 21, 30, 39, 48, 54, 55, 56, 58, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
        {  4, 13, 22, 31, 40, 49, 54, 55, 56, 57, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
        {  5, 14, 23, 32, 41, 50, 54, 55, 56, 57, 58, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
        {  6, 15, 24, 33, 42, 51, 54, 55, 56, 57, 58, 59, 61, 62, 69, 70, 71, 78, 79, 80 },
        {  7, 16, 25, 34, 43, 52, 54, 55, 56, 57, 58, 59, 60, 62, 69, 70, 71, 78, 79, 80 },
        {  8, 17, 26, 35, 44, 53, 54, 55, 56, 57, 58, 59, 60, 61, 69, 70, 71, 78, 79, 80 },
        {  0, 9,  18, 27, 36, 45, 54, 55, 56, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
        {  1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
        {  2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
        {  3, 12, 21, 30, 39, 48, 57, 58, 59, 63, 64, 65, 67, 68, 69, 70, 71, 75, 76, 77 },
        {  4, 13, 22, 31, 40, 49, 57, 58, 59, 63, 64, 65, 66, 68, 69, 70, 71, 75, 76, 77 },
        {  5, 14, 23, 32, 41, 50, 57, 58, 59, 63, 64, 65, 66, 67, 69, 70, 71, 75, 76, 77 },
        {  6, 15, 24, 33, 42, 51, 60, 61, 62, 63, 64, 65, 66, 67, 68, 70, 71, 78, 79, 80 },
        {  7, 16, 25, 34, 43, 52, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 78, 79, 80 },
        {  8, 17, 26, 35, 44, 53, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 78, 79, 80 },
        {  0, 9,  18, 27, 36, 45, 54, 55, 56, 63, 64, 65, 73, 74, 75, 76, 77, 78, 79, 80 },
        {  1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 64, 65, 72, 74, 75, 76, 77, 78, 79, 80 },
        {  2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 65, 72, 73, 75, 76, 77, 78, 79, 80 },
        {  3, 12, 21, 30, 39, 48, 57, 58, 59, 66, 67, 68, 72, 73, 74, 76, 77, 78, 79, 80 },
        {  4, 13, 22, 31, 40, 49, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 77, 78, 79, 80 },
        {  5, 14, 23, 32, 41, 50, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 76, 78, 79, 80 },
        {  6, 15, 24, 33, 42, 51, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 79, 80 },
        {  7, 16, 25, 34, 43, 52, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80 },
        {  8, 17, 26, 35, 44, 53, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 }
    };
    return DEPENDENTS_BY_INDEX[i];
}

std::string Board::locationName(int index)
{
    int r, c;
    locationOf(index, &r, &c);
    return locationName(r, c);
}

std::string Board::locationName(int r, int c)
{
    return std::string(1, rowName(r)) + columnName(c);
}

std::vector<int> const & Board::getRowIndexes(int r)
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

std::vector<int> const & Board::getColumnIndexes(int c)
{
    static std::vector<int> const INDEXES_BY_COLUMN[SIZE] =
    {
        { 0, 9,  18, 27, 36, 45, 54, 63, 72 },
        { 1, 10, 19, 28, 37, 46, 55, 64, 73 },
        { 2, 11, 20, 29, 38, 47, 56, 65, 74 },
        { 3, 12, 21, 30, 39, 48, 57, 66, 75 },
        { 4, 13, 22, 31, 40, 49, 58, 67, 76 },
        { 5, 14, 23, 32, 41, 50, 59, 68, 77 },
        { 6, 15, 24, 33, 42, 51, 60, 69, 78 },
        { 7, 16, 25, 34, 43, 52, 61, 70, 79 },
        { 8, 17, 26, 35, 44, 53, 62, 71, 80 }
    };

    return INDEXES_BY_COLUMN[c];
}

std::vector<int> const & Board::getBoxIndexes(int b)
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

bool Board::boxIsConsistent(int b) const
{
    int values = 0;
    for (int i : getBoxIndexes(b))
    {
        if (!consistent(get(i), values))
            return false;
    }
    return true;
}

bool Board::columnIsConsistent(int c) const
{
    int values = 0;
    for (int i : getColumnIndexes(c))
    {
        if (!consistent(get(i), values))
            return false;
    }
    return true;
}

bool Board::rowIsConsistent(int r) const
{
    int values = 0;
    for (int i : getRowIndexes(r))
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
