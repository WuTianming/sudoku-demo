#include "../sudokugame.h"

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>

using namespace std;

void test1() {
    SudokuGame game;
    int qwq[9][9];
    game.GeneratePuzzle();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            qwq[i][j] = game.at(i, j);
            printf("%d ", qwq[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    game.RemoveDigits(30);

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            qwq[i][j] = game.at(i, j);
            printf("%d ", qwq[i][j]);
        }
        printf("\n");
    }
    printf("valid = %d\n", game.isValid());

    game.GetSolution(&game.GetPuzzle(), SudokuGame::ASTAR);

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            qwq[i][j] = game.at(i, j);
            printf("%d ", qwq[i][j]);
        }
        printf("\n");
    }
    printf("valid = %d\n", game.isValid());
}

void test2() {
    int blank[9][9] = {
        {1,2,3,4,5,6,7,8,9},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
    };
    int nosolution[9][9] = {
        {6,1,2,0,0,0,7,0,0},
        {5,0,0,0,6,0,0,0,0},
        {0,0,0,0,0,0,0,9,0},
        {3,0,0,0,0,1,9,0,0},
        {0,0,0,0,0,4,0,7,0},
        {0,4,0,0,0,0,0,0,0},
        {0,0,8,0,0,0,0,0,2},
        {9,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,7}
    };
    int hard[9][9] = {
        {5,0,0,0,0,2,6,0,0},
        {9,0,0,0,3,0,0,0,2},
        {0,0,0,0,1,0,0,0,3},
        {0,0,2,0,7,6,0,1,0},
        {8,0,0,0,2,0,0,0,5},
        {0,5,0,0,0,4,9,0,0},
        {4,0,0,0,9,0,0,0,0},
        {1,0,0,0,6,0,0,0,8},
        {0,0,6,8,0,0,0,0,7}
    };
    int easy[9][9] = {
        {0,6,0,9,0,0,0,4,0},
        {0,4,0,0,7,0,0,8,0},
        {7,5,0,1,0,8,6,0,9},
        {2,0,0,0,9,0,7,0,0},
        {0,1,8,6,5,0,3,2,0},
        {0,0,4,0,8,0,0,0,1},
        {5,0,7,8,0,4,0,1,3},
        {0,9,0,0,1,0,0,5,0},
        {0,8,0,0,0,9,0,7,0}
    };
    int ka17[9][9] = {
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,3,0,8,5},
        {0,0,1,0,2,0,0,0,0},
        {0,0,0,5,0,7,0,0,0},
        {0,0,4,0,0,0,1,0,0},
        {0,9,0,0,0,0,0,0,0},
        {5,0,0,0,0,0,0,7,3},
        {0,0,2,0,1,0,0,0,0},
        {0,0,0,0,4,0,0,0,9}
    };
    int hardpp[9][9] = {
        {0,3,0,0,0,0,4,0,0},
        {0,0,4,0,0,5,0,0,0},
        {0,0,8,1,0,0,9,0,0},
        {0,0,3,0,0,7,0,5,0},
        {0,2,0,3,0,0,0,8,0},
        {0,9,0,0,0,0,3,0,0},
        {0,0,5,0,0,4,8,0,0},
        {0,0,0,2,0,0,6,0,0},
        {0,0,1,0,0,0,0,3,0}
    };
    int (*qwq)[9][9];
    qwq = &nosolution;
    clock_t bgn = clock();
    SudokuGame game;
    printf("suc = %d\n", game.GetSolution(qwq, SudokuGame::IDS));
    // printf("valid=%d\n", game.isValid());
    printf("time used = %lf\n",
            1.00 * (clock() - bgn) / CLOCKS_PER_SEC);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%d ", (*qwq)[i][j]);
        }
        printf("\n");
    }
}

void test3() {
    unsigned long seed;
    srand((unsigned long)new char*);
    clock_t bgn = clock();
    int k = 1;
    double timeused = 0.00, nowt;
    while (true) {
        seed = rand();
        // seed = 249804641;
        // seed = (unsigned long)new char*;
        // seed = 140686729953424ul;
        // seed = 409603424
        SudokuGame game(seed);
        game.GeneratePuzzle();
        assert(game.isValid());
        nowt = 1.00 * (clock() - bgn) / CLOCKS_PER_SEC;
        printf("cnt=%d, time used = %lf, delta time = %.2lfms\n", k, nowt, (nowt - timeused) * 1000);
        // printf("%d\n", game.isValid());
        // printf("%.2lf\n", (nowt - timeused) * 1000);
        timeused = nowt;
        k++;
        // int qwq[9][9];
        // printf("seed = %lu\n", seed);
        // getchar();
        // for (int i = 0; i < 9; i++) {
        //     for (int j = 0; j < 9; j++) {
        //         qwq[i][j] = game.at(i, j);
        //         printf("%d ", qwq[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
    }
}

int main() {
    test1();
    return 0;
}
