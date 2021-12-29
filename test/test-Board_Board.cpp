#include "Board/Board.h"

#include <gtest/gtest.h>

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

TEST(Board, Board)
{
    Board board;
    for (int i = 0; i < Board::NUM_CELLS; ++i)
    {
        EXPECT_EQ(board.get(i), 0);
    }
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

TEST(Board, initialize_i)
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

TEST(Board, DISABLED_solved)
{

}

TEST(Board, DISABLED_completed)
{

}

TEST(Board, DISABLED_consistent)
{

}

TEST(Board, DISABLED_draw)
{

}

TEST(Board, DISABLED_toJson)
{

}

TEST(Board_ForEach, DISABLED_cell)
{

}

TEST(Board_ForEach, DISABLED_row)
{

}

TEST(Board_ForEach, DISABLED_rowExcept1)
{

}

TEST(Board_ForEach, DISABLED_rowExcept2)
{

}

TEST(Board_ForEach, DISABLED_rowExcept3)
{

}

TEST(Board_ForEach, DISABLED_rowExcept4)
{

}

TEST(Board_ForEach, DISABLED_column)
{

}

TEST(Board_ForEach, DISABLED_columnExcept1)
{

}

TEST(Board_ForEach, DISABLED_columnExcept2)
{

}

TEST(Board_ForEach, DISABLED_columnExcept3)
{

}

TEST(Board_ForEach, DISABLED_columnExcept4)
{

}

TEST(Board_ForEach, DISABLED_box)
{

}

TEST(Board_ForEach, DISABLED_boxExcept1)
{

}

TEST(Board_ForEach, DISABLED_boxExcept2)
{

}

TEST(Board_ForEach, DISABLED_boxExcept3)
{

}

TEST(Board_ForEach, DISABLED_boxExcept4)
{

}

TEST(Board_ForEach, DISABLED_indexExcept1)
{

}

TEST(Board_ForEach, DISABLED_indexExcept2)
{

}

TEST(Board_ForEach, DISABLED_indexExcept3)
{

}

TEST(Board_ForEach, DISABLED_indexExcept4)
{

}

TEST(Board_Cell, DISABLED_next)
{

}

TEST(Board_Cell, DISABLED_dependents1)
{

}

TEST(Board_Cell, DISABLED_dependents2)
{

}

TEST(Board_Cell, DISABLED_indexOf)
{

}

TEST(Board_Cell, DISABLED_locationOf)
{

}

TEST(Board_Cell, DISABLED_name_index)
{

}

TEST(Board_Cell, DISABLED_name_r_c)
{

}

TEST(Board_Group, DISABLED_row)
{

}

TEST(Board_Group, DISABLED_column)
{

}

TEST(Board_Group, DISABLED_box)
{

}

TEST(Board_Group, DISABLED_whichRow)
{

}

TEST(Board_Group, DISABLED_whichColumn)
{

}

TEST(Board_Group, DISABLED_whichBox_r_c)
{

}

TEST(Board_Group, DISABLED_whichBox_index)
{

}

TEST(Board_Group, DISABLED_offsetInRow)
{

}

TEST(Board_Group, DISABLED_offsetInColumn)
{

}

TEST(Board_Group, DISABLED_offsetInBox)
{

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
