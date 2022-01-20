#include "Board/Board.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <random>

static char const EMPTY_BOARD_STRING[]  = "000000000000000000000000000000000000000000000000000000000000000000000000000000000";
static char const BOARD_TEST_1_STRING[] = "050749080089003000600001390040007060000400809000000000060004010500210047010005030";
static std::vector<int> const EMPTY_BOARD_VECTOR
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};
static std::vector<int> const BOARD_TEST_1_VECTOR
{
    0, 5, 0, 7, 4, 9, 0, 8, 0,
    0, 8, 9, 0, 0, 3, 0, 0, 0,
    6, 0, 0, 0, 0, 1, 3, 9, 0,
    0, 4, 0, 0, 0, 7, 0, 6, 0,
    0, 0, 0, 4, 0, 0, 8, 0, 9,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 6, 0, 0, 0, 4, 0, 1, 0,
    5, 0, 0, 2, 1, 0, 0, 4, 7,
    0, 1, 0, 0, 0, 5, 0, 3, 0
};

static std::vector<int> const SOLVED_BOARD_VECTOR
{
    8, 3, 4, 9, 6, 2, 1, 5, 7,
    6, 7, 1, 5, 3, 4, 9, 8, 2,
    5, 9, 2, 1, 7, 8, 4, 6, 3,
    4, 6, 7, 8, 5, 1, 2, 3, 9,
    9, 5, 3, 2, 4, 6, 7, 1, 8,
    1, 2, 8, 3, 9, 7, 5, 4, 6,
    7, 8, 6, 4, 2, 5, 3, 9, 1,
    2, 4, 9, 6, 1, 3, 8, 7, 5,
    3, 1, 5, 7, 8, 9, 6, 2, 4
};

TEST(Board, Board)
{
    Board board;
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), 0);
    }
}

TEST(Board, Board_v)
{
    EXPECT_NO_THROW
    ({
        Board board(BOARD_TEST_1_VECTOR);
    });
    EXPECT_ANY_THROW
    ({
        Board board = Board(std::vector<int>());
    });
    EXPECT_ANY_THROW
    ({
        std::vector<int> invalid(BOARD_TEST_1_VECTOR);
        invalid[0] = -1;
        Board board(invalid);
    });
    EXPECT_ANY_THROW
    ({
        Board board(std::vector<int>(Board::NUM_CELLS-1, Board::EMPTY)); // missing cell
    });
    EXPECT_ANY_THROW
    ({
        Board board(std::vector<int>(Board::NUM_CELLS+1, Board::EMPTY)); // extra cell
    });
    EXPECT_ANY_THROW
    ({
        Board board(std::vector<int>(Board::NUM_CELLS+1, -1));           // invalid value
    });
}

TEST(Board, Board_s)
{
    EXPECT_NO_THROW
    ({
        Board board(BOARD_TEST_1_STRING);
    });
    EXPECT_ANY_THROW
    ({
        Board board(nullptr);
    });
    EXPECT_ANY_THROW
    ({
        Board board("");
    });
    EXPECT_ANY_THROW
    ({
        Board board(std::string(BOARD_TEST_1_STRING).substr(0, Board::NUM_CELLS-1).c_str()); // missing cell
    });
    EXPECT_ANY_THROW
    ({
        Board board((std::string(BOARD_TEST_1_STRING) + '0').c_str());                       // extra cell
    });
    EXPECT_ANY_THROW
    ({
        Board board(std::string(BOARD_TEST_1_STRING).replace(0, 1, 1, 'x').c_str());        // non-digit
    });
}

TEST(Board, initialize_s)
{
    Board board;
    EXPECT_TRUE(board.initialize(BOARD_TEST_1_STRING));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), BOARD_TEST_1_STRING[i]-'0');
    }
    EXPECT_FALSE(board.initialize(nullptr));    // null pointer
    EXPECT_FALSE(board.initialize(""));         // empty string
    EXPECT_FALSE(board.initialize(std::string(BOARD_TEST_1_STRING).substr(0, Board::NUM_CELLS-1).c_str())); // missing cell
    EXPECT_FALSE(board.initialize((std::string(BOARD_TEST_1_STRING) + '0').c_str()));                       // extra cell
    EXPECT_FALSE(board.initialize(std::string(BOARD_TEST_1_STRING).replace(0, 1, 1, 'x').c_str()));        // non-digit
}

TEST(Board, initialize_v)
{
    Board board;
    EXPECT_TRUE(board.initialize(BOARD_TEST_1_VECTOR));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), BOARD_TEST_1_VECTOR[i]);
    }
    EXPECT_FALSE(board.initialize(std::vector<int>(Board::NUM_CELLS-1, Board::EMPTY))); // missing cell
    EXPECT_FALSE(board.initialize(std::vector<int>(Board::NUM_CELLS+1, Board::EMPTY))); // extra cell
    EXPECT_FALSE(board.initialize(std::vector<int>(Board::NUM_CELLS+1, -1)));           // invalid value
}

TEST(Board, cells)
{
    Board board;
    EXPECT_EQ(board.cells().size(), Board::NUM_CELLS);
    EXPECT_EQ(board.cells(), EMPTY_BOARD_VECTOR);

    board.initialize(BOARD_TEST_1_VECTOR);
    EXPECT_EQ(board.cells(), BOARD_TEST_1_VECTOR);

    board.initialize(SOLVED_BOARD_VECTOR);
    EXPECT_EQ(board.cells(), SOLVED_BOARD_VECTOR);
}

TEST(Board, serialize)
{
    Board board;
    std::string serialized;
    board.serialize(serialized);
    EXPECT_STREQ(serialized.c_str(), EMPTY_BOARD_STRING);

    board.initialize(BOARD_TEST_1_STRING);
    board.serialize(serialized);
    EXPECT_STREQ(serialized.c_str(), BOARD_TEST_1_STRING);
}

TEST(Board, get_set_r_c)
{
    Board board;
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_EQ(board.get(r, c), 0);
        }
    }

    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            board.set(r, c, BOARD_TEST_1_VECTOR[Board::Cell::indexOf(r, c)]);
        }
    }
    for (int r = Board::SIZE-1; r >= 0; --r)
    {
        for (int c = Board::SIZE-1; c >= 0; --c)
        {
            EXPECT_EQ(board.get(r, c), BOARD_TEST_1_VECTOR[Board::Cell::indexOf(r, c)]);
        }
    }
}

TEST(Board, get_set_i)
{
    Board board;
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), 0);
    }

    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        board.set(i, BOARD_TEST_1_VECTOR[i]);
    }

    for (int i = Board::NUM_CELLS-1; i >= 0; --i)
    {
        EXPECT_EQ(board.get(i), BOARD_TEST_1_VECTOR[i]);
    }
}

TEST(Board, isEmpty_r_c)
{
    Board board;
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_TRUE(board.isEmpty(r, c));
        }
    }

    board.initialize(std::vector<int>(Board::NUM_CELLS, 1));
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_FALSE(board.isEmpty(r, c));
        }
    }

    board.initialize(BOARD_TEST_1_VECTOR);
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_EQ(board.isEmpty(r, c), BOARD_TEST_1_VECTOR[Board::Cell::indexOf(r, c)] == Board::EMPTY);
        }
    }
}

TEST(Board, isEmpty_i)
{
    Board board;
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_TRUE(board.isEmpty(i));
    }

    board.initialize(std::vector<int>(Board::NUM_CELLS, 1));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_FALSE(board.isEmpty(i));
    }

    board.initialize(BOARD_TEST_1_VECTOR);
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.isEmpty(i), BOARD_TEST_1_VECTOR[i] == Board::EMPTY);
    }
}

TEST(Board, DISABLED_candidates_r_c)
{

}

TEST(Board, DISABLED_candidates_i)
{

}

TEST(Board, DISABLED_nextEmpty)
{

}

TEST(Board, solved)
{
    Board board;
    EXPECT_FALSE(board.solved());

    board.initialize(std::vector<int>(Board::NUM_CELLS, 1));
    EXPECT_FALSE(board.solved());

    board.initialize(BOARD_TEST_1_VECTOR);
    EXPECT_FALSE(board.solved());

    board.initialize(SOLVED_BOARD_VECTOR);
    EXPECT_TRUE(board.solved());
}

TEST(Board, completed)
{
    Board board;
    EXPECT_FALSE(board.completed());

    board.initialize(std::vector<int>(Board::NUM_CELLS, 1));
    EXPECT_TRUE(board.completed());

    board.initialize(BOARD_TEST_1_VECTOR);
    EXPECT_FALSE(board.completed());

    board.initialize(SOLVED_BOARD_VECTOR);
    EXPECT_TRUE(board.completed());
}

TEST(Board, consistent)
{
    Board board;
    EXPECT_TRUE(board.consistent());

    board.initialize(std::vector<int>(Board::NUM_CELLS, 1));
    EXPECT_FALSE(board.consistent());

    board.initialize(BOARD_TEST_1_VECTOR);
    EXPECT_TRUE(board.consistent());

    board.initialize(SOLVED_BOARD_VECTOR);
    EXPECT_TRUE(board.consistent());
}

TEST(Board, DISABLED_draw)
{

}

TEST(Board, DISABLED_toJson)
{

}

TEST(Board_ForEach, cell)
{
    Board board;

    // Test for every cell with correct index
    EXPECT_TRUE(Board::ForEach::cell([&board] (int i)
    {
        board.set(i, i);
        return true;
    }));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), i);
    }

    // Test abort
    EXPECT_FALSE(Board::ForEach::cell([] (int i)
    {
        return (i != 41);
    }));
}

TEST(Board_ForEach, row)
{
    Board board;

    // Test for every row and that the indexes in each row are correct
    EXPECT_TRUE(Board::ForEach::row([&board] (int r, std::vector<int> const & row)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_EQ(row[c], r * Board::SIZE + c);
            board.set(row[c], r * Board::SIZE + c);
        }
        return true;
    }));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), i);
    }

    // Test abort
    EXPECT_FALSE(Board::ForEach::row([] (int r, std::vector<int> const &)
    {
        return (r != 5);
    }));
}

TEST(Board_ForEach, rowExcept1)
{
    // For each possible excluded row
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        Board board;

        // Test for every row and that the indexes in each row are correct, verifying the excluded row
        EXPECT_TRUE(Board::ForEach::rowExcept(x0, [&board, x0] (int r, std::vector<int> const & row)
        {
            EXPECT_NE(r, x0);
            for (int c = 0; c < Board::SIZE; ++c)
            {
                EXPECT_EQ(row[c], r * Board::SIZE + c);
                board.set(row[c], r * Board::SIZE + c);
            }
            return true;
        }));
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_EQ(board.get(x0, c), 0);
            board.set(x0, c, x0 * Board::SIZE + c);
        }
        for (int i = 0; i < Board::NUM_CELLS; ++i)
        {
            EXPECT_EQ(board.get(i), i);
        }

        // Test abort
        EXPECT_FALSE(Board::ForEach::rowExcept(x0, [x0] (int, std::vector<int> const &)
        {
            return false;
        }));
    }
}

TEST(Board_ForEach, rowExcept2)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            Board board;

            // Test for every row and that the indexes in each row are correct, verifying the excluded row
            EXPECT_TRUE(Board::ForEach::rowExcept(x0, x1, [&board, x0, x1] (int r, std::vector<int> const & row)
            {
                EXPECT_NE(r, x0);
                EXPECT_NE(r, x1);
                for (int c = 0; c < Board::SIZE; ++c)
                {
                    EXPECT_EQ(row[c], r * Board::SIZE + c);
                    board.set(row[c], r * Board::SIZE + c);
                }
                return true;
            }));
            for (int c = 0; c < Board::SIZE; ++c)
            {
                EXPECT_EQ(board.get(x0, c), 0);
                EXPECT_EQ(board.get(x1, c), 0);
                board.set(x0, c, x0 * Board::SIZE + c);
                board.set(x1, c, x1 * Board::SIZE + c);
            }
            for (int i = 0; i < Board::NUM_CELLS; ++i)
            {
                EXPECT_EQ(board.get(i), i);
            }

            // Test abort
            EXPECT_FALSE(Board::ForEach::rowExcept(x0, x1, [] (int, std::vector<int> const &)
            {
                return false;
            }));
        }
    }
}

TEST(Board_ForEach, rowExcept3)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                Board board;

                // Test for every row and that the indexes in each row are correct, verifying the excluded row
                EXPECT_TRUE(Board::ForEach::rowExcept(x0, x1, x2, [&board, x0, x1, x2] (int r, std::vector<int> const & row)
                {
                    EXPECT_NE(r, x0);
                    EXPECT_NE(r, x1);
                    EXPECT_NE(r, x2);
                    for (int c = 0; c < Board::SIZE; ++c)
                    {
                        EXPECT_EQ(row[c], r * Board::SIZE + c);
                        board.set(row[c], r * Board::SIZE + c);
                    }
                    return true;
                }));
                for (int c = 0; c < Board::SIZE; ++c)
                {
                    EXPECT_EQ(board.get(x0, c), 0);
                    EXPECT_EQ(board.get(x1, c), 0);
                    EXPECT_EQ(board.get(x2, c), 0);
                    board.set(x0, c, x0 * Board::SIZE + c);
                    board.set(x1, c, x1 * Board::SIZE + c);
                    board.set(x2, c, x2 * Board::SIZE + c);
                }
                for (int i = 0; i < Board::NUM_CELLS; ++i)
                {
                    EXPECT_EQ(board.get(i), i);
                }

                // Test abort
                EXPECT_FALSE(Board::ForEach::rowExcept(x0, x1, x2, [] (int, std::vector<int> const &)
                {
                    return false;
                }));
            }
        }
    }
}

TEST(Board_ForEach, rowExcept4)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                for (int x3 = 0; x3 < Board::SIZE; ++x3)
                {
                    Board board;

                    // Test for every row and that the indexes in each row are correct, verifying the excluded row
                    EXPECT_TRUE(Board::ForEach::rowExcept(x0, x1, x2, x3, [&board, x0, x1, x2, x3] (int r, std::vector<int> const & row)
                    {
                        EXPECT_NE(r, x0);
                        EXPECT_NE(r, x1);
                        EXPECT_NE(r, x2);
                        EXPECT_NE(r, x3);
                        for (int c = 0; c < Board::SIZE; ++c)
                        {
                            EXPECT_EQ(row[c], r * Board::SIZE + c);
                            board.set(row[c], r * Board::SIZE + c);
                        }
                        return true;
                    }));
                    for (int c = 0; c < Board::SIZE; ++c)
                    {
                        EXPECT_EQ(board.get(x0, c), 0);
                        EXPECT_EQ(board.get(x1, c), 0);
                        EXPECT_EQ(board.get(x2, c), 0);
                        EXPECT_EQ(board.get(x3, c), 0);
                        board.set(x0, c, x0 * Board::SIZE + c);
                        board.set(x1, c, x1 * Board::SIZE + c);
                        board.set(x2, c, x2 * Board::SIZE + c);
                        board.set(x3, c, x3 * Board::SIZE + c);
                    }
                    for (int i = 0; i < Board::NUM_CELLS; ++i)
                    {
                        EXPECT_EQ(board.get(i), i);
                    }

                    // Test abort
                    EXPECT_FALSE(Board::ForEach::rowExcept(x0, x1, x2, x3, [] (int, std::vector<int> const &)
                    {
                        return false;
                    }));
                }
            }
        }
    }
}

TEST(Board_ForEach, column)
{
    Board board;

    // Test for every column and that the indexes in each column are correct
    EXPECT_TRUE(Board::ForEach::column([&board] (int c, std::vector<int> const & column)
    {
        for (int r = 0; r < Board::SIZE; ++r)
        {
            EXPECT_EQ(column[r], r * Board::SIZE + c);
            board.set(column[r], r * Board::SIZE + c);
        }
        return true;
    }));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), i);
    }

    // Test abort
    EXPECT_FALSE(Board::ForEach::column([] (int c, std::vector<int> const &)
    {
        return (c != 5);
    }));
}

TEST(Board_ForEach, columnExcept1)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        Board board;

        // Test for every column and that the indexes in each column are correct, verifying the excluded column
        EXPECT_TRUE(Board::ForEach::columnExcept(x0, [&board, x0] (int c, std::vector<int> const & column)
        {
            EXPECT_NE(c, x0);
            for (int r = 0; r < Board::SIZE; ++r)
            {
                EXPECT_EQ(column[r], r * Board::SIZE + c);
                board.set(column[r], r * Board::SIZE + c);
            }
            return true;
        }));
        for (int r = 0; r < Board::SIZE; ++r)
        {
            EXPECT_EQ(board.get(r, x0), 0);
            board.set(r, x0, r * Board::SIZE + x0);
        }
        for (int i = 0; i < Board::NUM_CELLS; ++i)
        {
            EXPECT_EQ(board.get(i), i);
        }

        // Test abort
        EXPECT_FALSE(Board::ForEach::columnExcept(x0, [] (int, std::vector<int> const &)
        {
            return false;
        }));
    }
}

TEST(Board_ForEach, columnExcept2)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            Board board;

            // Test for every column and that the indexes in each column are correct, verifying the excluded column
            EXPECT_TRUE(Board::ForEach::columnExcept(x0, x1, [&board, x0, x1] (int c, std::vector<int> const & column)
            {
                EXPECT_NE(c, x0);
                EXPECT_NE(c, x1);
                for (int r = 0; r < Board::SIZE; ++r)
                {
                    EXPECT_EQ(column[r], r * Board::SIZE + c);
                    board.set(column[r], r * Board::SIZE + c);
                }
                return true;
            }));
            for (int r = 0; r < Board::SIZE; ++r)
            {
                EXPECT_EQ(board.get(r, x0), 0);
                EXPECT_EQ(board.get(r, x1), 0);
                board.set(r, x0, r * Board::SIZE + x0);
                board.set(r, x1, r * Board::SIZE + x1);
            }
            for (int i = 0; i < Board::NUM_CELLS; ++i)
            {
                EXPECT_EQ(board.get(i), i);
            }

            // Test abort
            EXPECT_FALSE(Board::ForEach::columnExcept(x0, x1, [] (int, std::vector<int> const &)
            {
                return false;
            }));
        }
    }
}

TEST(Board_ForEach, columnExcept3)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                Board board;

                // Test for every column and that the indexes in each column are correct, verifying the excluded column
                EXPECT_TRUE(Board::ForEach::columnExcept(x0, x1, x2, [&board, x0, x1, x2] (int c, std::vector<int> const & column)
                {
                    EXPECT_NE(c, x0);
                    EXPECT_NE(c, x1);
                    EXPECT_NE(c, x2);
                    for (int r = 0; r < Board::SIZE; ++r)
                    {
                        EXPECT_EQ(column[r], r * Board::SIZE + c);
                        board.set(column[r], r * Board::SIZE + c);
                    }
                    return true;
                }));
                for (int r = 0; r < Board::SIZE; ++r)
                {
                    EXPECT_EQ(board.get(r, x0), 0);
                    EXPECT_EQ(board.get(r, x1), 0);
                    EXPECT_EQ(board.get(r, x2), 0);
                    board.set(r, x0, r * Board::SIZE + x0);
                    board.set(r, x1, r * Board::SIZE + x1);
                    board.set(r, x2, r * Board::SIZE + x2);
                }
                for (int i = 0; i < Board::NUM_CELLS; ++i)
                {
                    EXPECT_EQ(board.get(i), i);
                }

                // Test abort
                EXPECT_FALSE(Board::ForEach::columnExcept(x0, x1, x2, [] (int, std::vector<int> const &)
                {
                    return false;
                }));
            }
        }
    }
}

TEST(Board_ForEach, columnExcept4)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                for (int x3 = 0; x3 < Board::SIZE; ++x3)
                {
                    Board board;

                    // Test for every column and that the indexes in each column are correct, verifying the excluded column
                    EXPECT_TRUE(Board::ForEach::columnExcept(x0, x1, x2, x3, [&board, x0, x1, x2, x3] (int c, std::vector<int> const & column)
                    {
                        EXPECT_NE(c, x0);
                        EXPECT_NE(c, x1);
                        EXPECT_NE(c, x2);
                        EXPECT_NE(c, x3);
                        for (int r = 0; r < Board::SIZE; ++r)
                        {
                            EXPECT_EQ(column[r], r * Board::SIZE + c);
                            board.set(column[r], r * Board::SIZE + c);
                        }
                        return true;
                    }));
                    for (int r = 0; r < Board::SIZE; ++r)
                    {
                        EXPECT_EQ(board.get(r, x0), 0);
                        EXPECT_EQ(board.get(r, x1), 0);
                        EXPECT_EQ(board.get(r, x2), 0);
                        EXPECT_EQ(board.get(r, x3), 0);
                        board.set(r, x0, r * Board::SIZE + x0);
                        board.set(r, x1, r * Board::SIZE + x1);
                        board.set(r, x2, r * Board::SIZE + x2);
                        board.set(r, x3, r * Board::SIZE + x3);
                    }
                    for (int i = 0; i < Board::NUM_CELLS; ++i)
                    {
                        EXPECT_EQ(board.get(i), i);
                    }

                    // Test abort
                    EXPECT_FALSE(Board::ForEach::columnExcept(x0, x1, x2, x3, [] (int, std::vector<int> const &)
                    {
                        return false;
                    }));
                }
            }
        }
    }
}

TEST(Board_ForEach, box)
{
    Board board;

    // Test for every box and that the indexes in each box are correct
    EXPECT_TRUE(Board::ForEach::box([&board] (int b, std::vector<int> const & box)
    {
        for (int j = 0; j < Board::SIZE; ++j)
        {
            int i = (b / 3 * 3 + j / 3) * Board::SIZE + b % 3 * 3 + j % 3;
            EXPECT_EQ(box[j], i);
            board.set(box[j], i);
        }
        return true;
    }));
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), i);
    }

    // Test abort
    EXPECT_FALSE(Board::ForEach::box([] (int b, std::vector<int> const &)
    {
        return (b != 5);
    }));
}

TEST(Board_ForEach, boxExcept1)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        Board board;

        // Test for every box and that the indexes in each box are correct, verifying the excluded box
        EXPECT_TRUE(Board::ForEach::boxExcept(x0, [&board, x0] (int b, std::vector<int> const & box)
        {
            EXPECT_NE(b, x0);
            for (int j = 0; j < Board::SIZE; ++j)
            {
                int i = (b / 3 * 3 + j / 3) * Board::SIZE + b % 3 * 3 + j % 3;
                EXPECT_EQ(box[j], i);
                board.set(box[j], i);
            }
            return true;
        }));
        for (int j = 0; j < Board::SIZE; ++j)
        {
            int i0 = (x0 / 3 * 3 + j / 3) * Board::SIZE + x0 % 3 * 3 + j % 3;
            EXPECT_EQ(board.get(i0), 0);
            board.set(i0, i0);
        }
        for (int i = 0; i < Board::NUM_CELLS; ++i)
        {
            EXPECT_EQ(board.get(i), i);
        }

        // Test abort
        EXPECT_FALSE(Board::ForEach::boxExcept(x0, [] (int, std::vector<int> const &)
        {
            return false;
        }));
    }
}

TEST(Board_ForEach, boxExcept2)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            Board board;

            // Test for every box and that the indexes in each box are correct, verifying the excluded box
            EXPECT_TRUE(Board::ForEach::boxExcept(x0, x1, [&board, x0, x1] (int b, std::vector<int> const & box)
            {
                EXPECT_NE(b, x0);
                EXPECT_NE(b, x1);
                for (int j = 0; j < Board::SIZE; ++j)
                {
                    int i = (b / 3 * 3 + j / 3) * Board::SIZE + b % 3 * 3 + j % 3;
                    EXPECT_EQ(box[j], i);
                    board.set(box[j], i);
                }
                return true;
            }));
            for (int j = 0; j < Board::SIZE; ++j)
            {
                int i0 = (x0 / 3 * 3 + j / 3) * Board::SIZE + x0 % 3 * 3 + j % 3;
                int i1 = (x1 / 3 * 3 + j / 3) * Board::SIZE + x1 % 3 * 3 + j % 3;
                EXPECT_EQ(board.get(i0), 0);
                EXPECT_EQ(board.get(i1), 0);
                board.set(i0, i0);
                board.set(i1, i1);
            }
            for (int i = 0; i < Board::NUM_CELLS; ++i)
            {
                EXPECT_EQ(board.get(i), i);
            }

            // Test abort
            EXPECT_FALSE(Board::ForEach::boxExcept(x0, x1, [] (int, std::vector<int> const &)
            {
                return false;
            }));
        }
    }
}

TEST(Board_ForEach, boxExcept3)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                Board board;

                // Test for every box and that the indexes in each box are correct, verifying the excluded box
                EXPECT_TRUE(Board::ForEach::boxExcept(x0, x1, x2, [&board, x0, x1, x2] (int b, std::vector<int> const & box)
                {
                    EXPECT_NE(b, x0);
                    EXPECT_NE(b, x1);
                    EXPECT_NE(b, x2);
                    for (int j = 0; j < Board::SIZE; ++j)
                    {
                        int i = (b / 3 * 3 + j / 3) * Board::SIZE + b % 3 * 3 + j % 3;
                        EXPECT_EQ(box[j], i);
                        board.set(box[j], i);
                    }
                    return true;
                }));
                for (int j = 0; j < Board::SIZE; ++j)
                {
                    int i0 = (x0 / 3 * 3 + j / 3) * Board::SIZE + x0 % 3 * 3 + j % 3;
                    int i1 = (x1 / 3 * 3 + j / 3) * Board::SIZE + x1 % 3 * 3 + j % 3;
                    int i2 = (x2 / 3 * 3 + j / 3) * Board::SIZE + x2 % 3 * 3 + j % 3;
                    EXPECT_EQ(board.get(i0), 0);
                    EXPECT_EQ(board.get(i1), 0);
                    EXPECT_EQ(board.get(i2), 0);
                    board.set(i0, i0);
                    board.set(i1, i1);
                    board.set(i2, i2);
                }
                for (int i = 0; i < Board::NUM_CELLS; ++i)
                {
                    EXPECT_EQ(board.get(i), i);
                }

                // Test abort
                EXPECT_FALSE(Board::ForEach::boxExcept(x0, x1, x2, [] (int, std::vector<int> const &)
                {
                    return false;
                }));
            }
        }
    }
}

TEST(Board_ForEach, boxExcept4)
{
    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                for (int x3 = 0; x3 < Board::SIZE; ++x3)
                {
                    Board board;

                    // Test for every box and that the indexes in each box are correct, verifying the excluded box
                    EXPECT_TRUE(Board::ForEach::boxExcept(x0, x1, x2, x3, [&board, x0, x1, x2, x3] (int b, std::vector<int> const & box)
                    {
                        EXPECT_NE(b, x0);
                        EXPECT_NE(b, x1);
                        EXPECT_NE(b, x2);
                        EXPECT_NE(b, x3);
                        for (int j = 0; j < Board::SIZE; ++j)
                        {
                            int i = (b / 3 * 3 + j / 3) * Board::SIZE + b % 3 * 3 + j % 3;
                            EXPECT_EQ(box[j], i);
                            board.set(box[j], i);
                        }
                        return true;
                    }));
                    for (int j = 0; j < Board::SIZE; ++j)
                    {
                        int i0 = (x0 / 3 * 3 + j / 3) * Board::SIZE + x0 % 3 * 3 + j % 3;
                        int i1 = (x1 / 3 * 3 + j / 3) * Board::SIZE + x1 % 3 * 3 + j % 3;
                        int i2 = (x2 / 3 * 3 + j / 3) * Board::SIZE + x2 % 3 * 3 + j % 3;
                        int i3 = (x3 / 3 * 3 + j / 3) * Board::SIZE + x3 % 3 * 3 + j % 3;
                        EXPECT_EQ(board.get(i0), 0);
                        EXPECT_EQ(board.get(i1), 0);
                        EXPECT_EQ(board.get(i2), 0);
                        EXPECT_EQ(board.get(i3), 0);
                        board.set(i0, i0);
                        board.set(i1, i1);
                        board.set(i2, i2);
                        board.set(i3, i3);
                    }
                    for (int i = 0; i < Board::NUM_CELLS; ++i)
                    {
                        EXPECT_EQ(board.get(i), i);
                    }

                    // Test abort
                    EXPECT_FALSE(Board::ForEach::boxExcept(x0, x1, x2, x3, [] (int, std::vector<int> const &)
                    {
                        return false;
                    }));
                }
            }
        }
    }
}

TEST(Board_ForEach, indexExcept1)
{
    std::vector<int> all(Board::NUM_CELLS);
    std::iota(all.begin(), all.end(), 0);
    std::shuffle(all.begin(), all.end(), std::mt19937{std::random_device{}()});

    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        Board board;

        // Test for every box and that the indexes in each box are correct, verifying the excluded box
        EXPECT_TRUE(Board::ForEach::indexExcept(all, x0, [&board, x0] (int i)
        {
            EXPECT_NE(i, x0);
            board.set(i, i);
            return true;
        }));
        EXPECT_EQ(board.get(x0), 0);
        board.set(x0, x0);
        for (int i = 0; i < Board::NUM_CELLS; ++i)
        {
            EXPECT_EQ(board.get(i), i);
        }

        // Test abort
        EXPECT_FALSE(Board::ForEach::indexExcept(all, x0, [] (int)
        {
            return false;
        }));
    }
}

TEST(Board_ForEach, indexExcept2)
{
    std::vector<int> all(Board::NUM_CELLS);
    std::iota(all.begin(), all.end(), 0);
    std::shuffle(all.begin(), all.end(), std::mt19937{std::random_device{}()});

    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            Board board;

            // Test for every box and that the indexes in each box are correct, verifying the excluded box
            EXPECT_TRUE(Board::ForEach::indexExcept(all, x0, x1, [&board, x0, x1] (int i)
            {
                EXPECT_NE(i, x0);
                EXPECT_NE(i, x1);
                board.set(i, i);
                return true;
            }));
            EXPECT_EQ(board.get(x0), 0);
            EXPECT_EQ(board.get(x1), 0);
            board.set(x0, x0);
            board.set(x1, x1);
            for (int i = 0; i < Board::NUM_CELLS; ++i)
            {
                EXPECT_EQ(board.get(i), i);
            }

            // Test abort
            EXPECT_FALSE(Board::ForEach::indexExcept(all, x0, x1, [] (int)
            {
                return false;
            }));
        }
    }
}

TEST(Board_ForEach, indexExcept3)
{
    std::vector<int> all(Board::NUM_CELLS);
    std::iota(all.begin(), all.end(), 0);
    std::shuffle(all.begin(), all.end(), std::mt19937{std::random_device{}()});

    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                for (int x3 = 0; x3 < Board::SIZE; ++x3)
                {
                    Board board;

                    // Test for every box and that the indexes in each box are correct, verifying the excluded box
                    EXPECT_TRUE(Board::ForEach::indexExcept(all, x0, x1, x2, [&board, x0, x1, x2] (int i)
                    {
                        EXPECT_NE(i, x0);
                        EXPECT_NE(i, x1);
                        EXPECT_NE(i, x2);
                        board.set(i, i);
                        return true;
                    }));
                    EXPECT_EQ(board.get(x0), 0);
                    EXPECT_EQ(board.get(x1), 0);
                    EXPECT_EQ(board.get(x2), 0);
                    board.set(x0, x0);
                    board.set(x1, x1);
                    board.set(x2, x2);
                    for (int i = 0; i < Board::NUM_CELLS; ++i)
                    {
                        EXPECT_EQ(board.get(i), i);
                    }

                    // Test abort
                    EXPECT_FALSE(Board::ForEach::indexExcept(all, x0, x1, x2, [] (int)
                    {
                        return false;
                    }));
                }
            }
        }
    }
}

TEST(Board_ForEach, indexExcept4)
{
    std::vector<int> all(Board::NUM_CELLS);
    std::iota(all.begin(), all.end(), 0);
    std::shuffle(all.begin(), all.end(), std::mt19937{std::random_device{}()});

    for (int x0 = 0; x0 < Board::SIZE; ++x0)
    {
        for (int x1 = 0; x1 < Board::SIZE; ++x1)
        {
            for (int x2 = 0; x2 < Board::SIZE; ++x2)
            {
                for (int x3 = 0; x3 < Board::SIZE; ++x3)
                {
                    Board board;

                    // Test for every box and that the indexes in each box are correct, verifying the excluded box
                    EXPECT_TRUE(Board::ForEach::indexExcept(all, x0, x1, x2, x3, [&board, x0, x1, x2, x3] (int i)
                    {
                        EXPECT_NE(i, x0);
                        EXPECT_NE(i, x1);
                        EXPECT_NE(i, x2);
                        EXPECT_NE(i, x3);
                        board.set(i, i);
                        return true;
                    }));
                    EXPECT_EQ(board.get(x0), 0);
                    EXPECT_EQ(board.get(x1), 0);
                    EXPECT_EQ(board.get(x2), 0);
                    EXPECT_EQ(board.get(x3), 0);
                    board.set(x0, x0);
                    board.set(x1, x1);
                    board.set(x2, x2);
                    board.set(x3, x3);
                    for (int i = 0; i < Board::NUM_CELLS; ++i)
                    {
                        EXPECT_EQ(board.get(i), i);
                    }

                    // Test abort
                    EXPECT_FALSE(Board::ForEach::indexExcept(all, x0, x1, x2, x3, [] (int)
                    {
                        return false;
                    }));
                }
            }
        }
    }
}

TEST(Board_Cell, next)
{
    int expected_r = 0;
    int expected_c = 1;
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            int next_r = r;
            int next_c = c;
            Board::Cell::next(&next_r, &next_c);
            EXPECT_EQ(next_r, expected_r);
            EXPECT_EQ(next_c, expected_c);
            if (++expected_c >= Board::SIZE)
            {
                expected_c = 0;
                ++expected_r;
            }
        }
    }
}

TEST(Board_Cell, DISABLED_dependents1)
{

}

TEST(Board_Cell, DISABLED_dependents2)
{

}

TEST(Board_Cell, indexOf)
{
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_EQ(Board::Cell::indexOf(r, c), r * Board::SIZE + c);
        }
    }
}

TEST(Board_Cell, locationOf)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        int r = -1;
        int c = -1;
        Board::Cell::locationOf(i, &r, &c);
        EXPECT_EQ(r, i / Board::SIZE);
        EXPECT_EQ(c, i % Board::SIZE);
    }
}

TEST(Board_Cell, name_index)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        int r = i / Board::SIZE;
        int c = i % Board::SIZE;
        EXPECT_EQ(Board::Cell::name(i), std::string(1, "ABCDEFGHJ"[r]) + std::to_string(c+1));
    }
}

TEST(Board_Cell, name_r_c)
{
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            EXPECT_EQ(Board::Cell::name(r, c), std::string(1, "ABCDEFGHJ"[r]) + std::to_string(c+1));
        }
    }
}

TEST(Board_Group, row)
{
    std::vector<int> row(Board::SIZE);
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::iota(row.begin(), row.end(), r * Board::SIZE);
        EXPECT_EQ(Board::Group::row(r), row);
    }
}

TEST(Board_Group, column)
{
    std::vector<int> column(Board::SIZE);
    for (int c = 0; c < Board::SIZE; ++c)
    {
        for (int i = 0; i < column.size(); ++i)
        {
            column[i] = c + i * Board::SIZE;
        }
        EXPECT_EQ(Board::Group::column(c), column);
    }
}

TEST(Board_Group, box)
{
    std::vector<int> box(Board::SIZE);
    for (int b = 0; b < Board::SIZE; ++b)
    {
        int by = b / 3;
        int bx = b % 3;
        for (int i = 0; i < Board::SIZE; ++i)
        {
            int r = i / 3;
            int c = i % 3;
            box[i] = (by * 3 + r) * Board::SIZE + bx * 3 + c;
        }
        EXPECT_EQ(Board::Group::box(b), box);
    }
}

TEST(Board_Group, whichRow)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(Board::Group::whichRow(i), i / Board::SIZE);
    }
}

TEST(Board_Group, whichColumn)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(Board::Group::whichColumn(i), i % Board::SIZE);
    }
}

TEST(Board_Group, whichBox_r_c)
{
    for (int r = 0; r < Board::SIZE; ++r)
    {
        for (int c = 0; c < Board::SIZE; ++c)
        {
            int by = r / 3;
            int bx = c / 3;
            EXPECT_EQ(Board::Group::whichBox(r, c), by * 3 + bx);
        }
    }
}

TEST(Board_Group, whichBox_index)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        int by = i / Board::SIZE / 3;
        int bx = i % Board::SIZE / 3;
        EXPECT_EQ(Board::Group::whichBox(i), by * 3 + bx);
    }
}

TEST(Board_Group, offsetInRow)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(Board::Group::offsetInRow(i), i % Board::SIZE);
    }
}

TEST(Board_Group, offsetInColumn)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(Board::Group::offsetInColumn(i), i / Board::SIZE);
    }
}

TEST(Board_Group, offsetInBox)
{
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        int r = i / Board::SIZE % 3;
        int c = i % Board::SIZE % 3;
        EXPECT_EQ(Board::Group::offsetInBox(i), r * 3 + c);
    }
}

TEST(Board_Group, DISABLED_rowName)
{

}

TEST(Board_Group, DISABLED_columnName)
{

}

TEST(Board_Group, DISABLED_boxName)
{

}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
