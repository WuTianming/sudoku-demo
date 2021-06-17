#include "sudokunode.h"

SudokuNode::SudokuNode()
{
    memset(a, 0, sizeof(int) * 81);
    memset(flag, 0, sizeof(uint16_t) * 36);
}

SudokuNode::SudokuNode(int src[9][9])
{
    memcpy(a, src, sizeof(int) * 81);
    memset(flag, 0, sizeof(uint16_t) * 36);
    cal_cost();
}

double &SudokuNode::cal_cost()
{
    cost = 0.00;
    SudokuGame::cal_flag(this->a, this->flag);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (!a[i][j])
                cost += 9 - SudokuGame::count_bits(SudokuGame::get_flag(flag, i, j));
    cost *= 0.25;
    return cost;
}
