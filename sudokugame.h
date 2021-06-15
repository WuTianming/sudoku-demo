#ifndef SUDOKUGAME_H
#define SUDOKUGAME_H

#include <ctime>
#include <stack>
#include <random>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>

class SudokuGame
{
public:
    SudokuGame();
    SudokuGame(unsigned seed);
    void GeneratePuzzle();
    void RemoveDigits(int remain);

    auto &GetPuzzle() { return this->gameBoard; }
    void SetPuzzle(const int (*p)[9][9]);

    int  at(int i, int j);
    // bool isGiven(int i, int j);
    bool isValid(const int (*ans)[9][9] = NULL);

    enum {
        STABLE_DFS = 1, DFS, IDS, ASTAR, IDASTAR, DLX,
        PARALLEL_DFS = 10
    };
    bool GetSolution(int (*p)[9][9], int MOD = 0);

    void Save(const char *filename, const int (*current)[9][9]);
    bool Read(const char *filename, int (*current)[9][9]);

private:
    int  gameBoard[9][9];
    // bool given[9][9];
    std::default_random_engine rand;
    bool GenSolvable_rand17(int s[9][9], int dep);
    int  count_bits(int t);
    bool dfs(int stk[9][9], uint16_t flag[3][9], int i0, int j0);
    bool ids(int stk[9][9], uint16_t flag[3][9],
             int nowdep, int maxdep, int &dep_reached);
};

#endif // SUDOKUGAME_H
