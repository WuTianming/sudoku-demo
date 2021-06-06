#include "sudokugame.h"

// the constructor in turn calls the constructor of the random generator.
SudokuGame::SudokuGame() : rand(QDateTime::currentMSecsSinceEpoch())
{
    // generates the puzzle at startup, then randomly remove some digits.
    GeneratePuzzle();
    RemoveDigits(81 - 3);
}

// generates valid filled Sudoku puzzles
void SudokuGame::GeneratePuzzle()
{
    int Gen[9][9];
    memset(Gen, 0, sizeof(Gen));
    memset(gameBoard, 0, sizeof(gameBoard));
    memset(given, true, sizeof(given));

    // shuffle all the digits as they are equivalent
    int digits[10] = {0,1,2,3,4,5,6,7,8,9};
    std::shuffle(digits + 1, digits + 10, rand);

    // fill in the puzzle using "group filling and circular shift" strategy
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            Gen[i][j] = digits[(i*3 + j + i/3) % 9 + 1];
    // then shuffle every 3 columns and every 3 rows
    int shuffle3_1[9] = {0,1,2,3,4,5,6,7,8};
    int shuffle3_2[9] = {0,1,2,3,4,5,6,7,8};
    for (int i = 0; i < 3; i++) {
        std::shuffle(shuffle3_1 + 3 * i, shuffle3_1 + 3 + 3 * i, rand);
        std::shuffle(shuffle3_2 + 3 * i, shuffle3_2 + 3 + 3 * i, rand);
    }
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            gameBoard[shuffle3_1[i]][shuffle3_2[j]] = Gen[i][j];
}

// digs holes in gameBoard so that certain cells are left out
void SudokuGame::RemoveDigits(int remain)
{
    while (remain < 81) {
        int i = rand() % 9, j = rand() % 9;
        if (given[i][j]) {
            given[i][j] = false;
            remain++;
        }
    }
    return;
}

int SudokuGame::at(int i, int j)
{
    if (0 <= i && i < 9 && 0 <= j && j < 9)
        return gameBoard[i][j];
    else
        return 0;
}

bool SudokuGame::isGiven(int i, int j)
{
    return given[i][j];
}

bool SudokuGame::isValid(const int (*ans)[9][9])
{
    if (ans == NULL) ans = &gameBoard;
    int cnt[10], tot = 0;
    memset(cnt, 0, sizeof(cnt)); cnt[0] = -10000;
    ++tot;
    for (int i = 0; i < 9; i++, tot++)
        for (int j = 0; j < 9; j++)
            if (cnt[(*ans)[i][j]] != tot - 1)
                return false;
            else
                cnt[(*ans)[i][j]] = tot;
    for (int i = 0; i < 9; i++, tot++)
        for (int j = 0; j < 9; j++)
            if (cnt[(*ans)[i][j]] != tot - 1)
                return false;
            else
                cnt[(*ans)[i][j]] = tot;
    for (int i = 0; i < 9; i += 3)
        for (int j = 0; j < 9; j += 3, tot++)
            for (int k = i; k < i + 3; k++)
                for (int l = j; l < j + 3; l++)
                    if (cnt[(*ans)[k][l]] != tot - 1)
                        return false;
                    else
                        cnt[(*ans)[k][l]] = tot;
    return true;
}

void SudokuGame::SetPuzzle(const int (*p)[9][9])
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            this->given[i][j] = !!(*p)[i][j];
            this->gameBoard[i][j] = (*p)[i][j];
        }
    }
}
