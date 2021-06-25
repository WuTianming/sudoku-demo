#ifndef SUDOKUGAME_H
#define SUDOKUGAME_H

#include "sudokunode.h"

#include <ctime>
#include <queue>
#include <stack>
#include <cstdio>
#include <random>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <pthread.h>

#define XSUDOKU
class SudokuGame
{
public:
    SudokuGame();
    SudokuGame(unsigned seed);
    void GeneratePuzzle(bool filled = true, int cnt = 17);
    void RemoveDigits(int remain);

    auto &GetPuzzle() { return this->gameBoard; }
    void SetPuzzle(const int (*p)[9][9]);

    int  at(int i, int j);
    // bool isGiven(int i, int j);
    bool isValid();
    static bool verify(const int ans[9][9]);
    static bool cal_flag(const int s[9][9], uint16_t flag[4][9]);

    enum {
        STABLE_DFS = 0, DFS, IDS, ASTAR, IDASTAR = 10, DLX,
        PARALLEL_DFS = 4
    };
    static int  GetSolution(int (*p)[9][9], int MOD = 1);

    void Save(const char *filename, const int (*current)[9][9]);
    bool Read(const char *filename, int (*current)[9][9]);

private:
    friend class SudokuNode;

    int  gameBoard[9][9];
    // bool given[9][9];
    std::default_random_engine rand;
    bool GenRand(int s[9][9], uint16_t flag[4][9], int cnt = 17, bool solvable = true);
    bool GenSolvable_rand17(int s[9][9], int dep = 9);

    static inline int  count_bits(int t) {
#define lowbit(t) ((t) & (-(t)))
        int cnt = 0;
        while (t) {
            t ^= lowbit(t);
            ++cnt;
        }
        return cnt;
#undef lowbit
    }
    static inline uint16_t get_flag(uint16_t flag[4][9], int i, int j) {
        uint16_t fff = flag[0][(i/3) * 3 + j/3] | flag[1][j] | flag[2][i];
#ifdef XSUDOKU
        if (i == j) fff |= flag[3][0];
        if (i+j==8) fff |= flag[3][1];
#endif
        return fff;
    }
    static inline void set_flag(uint16_t flag[4][9], int i, int j, int k) {
        if (!k) return;
        uint16_t bit = 1u << k;
        flag[0][(i/3) * 3 + j/3] ^= bit;
        flag[1][j] ^= bit;
        flag[2][i] ^= bit;
#ifdef XSUDOKU
        if (i == j) flag[3][0] ^= bit;
        if (i+j==8) flag[3][1] ^= bit;
#endif
    }
    static bool dfs(int stk[9][9], uint16_t flag[4][9], int i0, int j0);
    static bool ids(int stk[9][9], uint16_t flag[4][9],
                    int nowdep, int maxdep, int &dep_reached);
    static bool astar(int stk[9][9]);
};

#endif // SUDOKUGAME_H
