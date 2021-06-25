#ifndef SUDOKUNODE_H
#define SUDOKUNODE_H

//#include "sudokugame.h"

#include <ctime>
#include <queue>
#include <stack>
#include <random>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>

class SudokuNode
{
public:
    double cost;
    int a[9][9];
    uint16_t flag[4][9];

    SudokuNode();
    SudokuNode(int src[9][9]);
    double &cal_cost();
    bool operator<(const SudokuNode &b) const {
        return this->cost > b.cost;
    }
};

#endif // SUDOKUNODE_H
