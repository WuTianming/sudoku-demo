#ifndef SUDOKUGAME_H
#define SUDOKUGAME_H

#include <QtDebug>
#include <QDateTime>

#include <ctime>
#include <random>
#include <cstring>
#include <iostream>
#include <algorithm>

class SudokuGame
{
public:
    SudokuGame();
    void GeneratePuzzle();
    void RemoveDigits(int remain);              // just remove randomly, as uniqueness isn't required

    void SetPuzzle(const int (*p)[9][9]);

    int  at(int i, int j);
    bool isGiven(int i, int j);
    bool isValid(const int (*ans)[9][9] = NULL);

private:
    int gameBoard[9][9];
    bool given[9][9];
    std::default_random_engine rand;
};

#endif // SUDOKUGAME_H
