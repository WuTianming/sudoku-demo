#include "sudokugame.h"
#include <cstdio>

// the constructor in turn calls the constructor of the random generator.
SudokuGame::SudokuGame() : rand(time(NULL)) {}
SudokuGame::SudokuGame(unsigned seed) : rand(seed) {}
// SudokuGame::SudokuGame() : rand(QDateTime::currentMSecsSinceEpoch())

// try to generate valid filled Sudoku puzzles using rand17 & solve method
bool SudokuGame::GenSolvable_rand17(int s[9][9], int dep = 8)
{
    uint16_t flag[4][9];
    memset(s, 0, sizeof(int) * 81);
    memset(flag, 0, sizeof(flag));
    for (int T = 1; T <= 17; T++) {
        int i = rand() % 9, j = rand() % 9;
        while (s[i][j]) { i = rand() % 9, j = rand() % 9; }
        int id = (i/3) * 3 + j/3;
        uint16_t fff = flag[0][id] | flag[1][j] | flag[2][i];
        if (i == j) fff |= flag[3][0];
        if (i+j==8) fff |= flag[3][1];
        int cnt = count_bits(fff);
        if (cnt == 9) return false;
        int it = rand() % (9 - cnt) + 1;
        for (int k = 1; k <= 9; k++) {
            // count until the it-th vacant number
            if (!(fff & (1u << k))) it--;
            if (!it) {
                // then fill in
                s[i][j] = k;
                flag[0][id] ^= (1u << k);
                flag[1][j]  ^= (1u << k);
                flag[2][i]  ^= (1u << k);
                if (i == j) flag[3][0] ^= (1u << k);
                if (i+j==8) flag[3][1] ^= (1u << k);
                break;
            }
        }
    }
    int reached = 0;
    for (int k = std::min(5, dep - 4); k <= dep; k++, reached = 0) {
        if (this->ids(s, flag, 1, k, reached)) { return true; }
        if (reached != k) { return false; }
    }
    return false;
}

// generate valid filled Sudoku puzzles
void SudokuGame::GeneratePuzzle()
{
    // int Gen[9][9];
    // memset(Gen, 0, sizeof(Gen));
    memset(gameBoard, 0, sizeof(gameBoard));
    // int digits[10] = {0,1,2,3,4,5,6,7,8,9};
    // std::shuffle(digits + 1, digits + 10, rand);
    // maybe something like a randomized IDS?
    int i = 0;
    int per[] = { 7, 7, 9 };
    while (!GenSolvable_rand17(gameBoard, per[i])) { (++i) %= 3; }

    /* shuffle Gen[][]
    int shuffle3_1[9] = {0,1,2,3,4,5,6,7,8};
    int shuffle3_2[9] = {0,1,2,3,4,5,6,7,8};
    for (int i = 0; i < 3; i++) {
        std::shuffle(shuffle3_1 + 3 * i, shuffle3_1 + 3 + 3 * i, rand);
        std::shuffle(shuffle3_2 + 3 * i, shuffle3_2 + 3 + 3 * i, rand);
    }
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            gameBoard[shuffle3_1[i]][shuffle3_2[j]] = Gen[i][j];
    */
}

// dig holes in gameBoard so that certain cells are left out
void SudokuGame::RemoveDigits(int remain)
{
    while (remain < 81) {
        int i = rand() % 9, j = rand() % 9;
        if (gameBoard[i][j]) {
            gameBoard[i][j] = 0;
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

bool SudokuGame::isValid(const int (*ans)[9][9])
{
    if (ans == NULL) ans = &gameBoard;
    int cnt[10], tot = 0;
    memset(cnt, 0, sizeof(cnt)); cnt[0] = -10000;
    ++tot;
    // check two diagonals
    for (int i = 0; i < 9; i++)
        if (cnt[(*ans)[i][i]] != tot - 1) return false;
        else cnt[(*ans)[i][i]] = tot;
    ++tot;
    for (int i = 0; i < 9; i++)
        if (cnt[(*ans)[i][9-i]] != tot - 1) return false;
        else cnt[(*ans)[i][9-i]] = tot;
    ++tot;
    // check every row
    for (int i = 0; i < 9; i++, tot++)
        for (int j = 0; j < 9; j++)
            if (cnt[(*ans)[i][j]] != tot - 1) return false;
            else cnt[(*ans)[i][j]] = tot;
    // check every column
    for (int j = 0; j < 9; j++, tot++)
        for (int i = 0; i < 9; i++)
            if (cnt[(*ans)[i][j]] != tot - 1) return false;
            else cnt[(*ans)[i][j]] = tot;
    // check every block
    for (int i = 0; i < 9; i += 3)
        for (int j = 0; j < 9; j += 3, tot++)
            for (int k = i; k < i + 3; k++)
                for (int l = j; l < j + 3; l++)
                    if (cnt[(*ans)[k][l]] != tot - 1) return false;
                    else cnt[(*ans)[k][l]] = tot;
    return true;
}

// copy gameboard from external data
void SudokuGame::SetPuzzle(const int (*p)[9][9])
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            this->gameBoard[i][j] = (*p)[i][j];
}

// naive DFS with fixed maxdepth
bool SudokuGame::dfs(int stk[9][9],
                     uint16_t flag[4][9], int i0, int j0)
{
    for (int i = i0; i < 9; i++) {
        for (int j = ((i == i0) ? j0 + 1 : 0); j < 9; j++) {
            if (!stk[i][j]) {
                int id = (i/3) * 3 + j/3;
                uint16_t fff = flag[0][id] | flag[1][j] | flag[2][i];
                if (i == j) fff |= flag[3][0];
                if (i+j==8) fff |= flag[3][1];
                for (int k = 1; k <= 9; k++) {
                    if (fff & (1u << k)) continue;
                    flag[0][id] ^= (1u << k);
                    flag[1][j]  ^= (1u << k);
                    flag[2][i]  ^= (1u << k);
                    if (i == j) flag[3][0] ^= (1u << k);
                    if (i+j==8) flag[3][1] ^= (1u << k);
                    stk[i][j] = k;
                    bool f = dfs(stk, flag, i, j);
                    if (f) { return true; }
                    flag[0][id] ^= (1u << k);
                    flag[1][j]  ^= (1u << k);
                    flag[2][i]  ^= (1u << k);
                    if (i == j) flag[3][0] ^= (1u << k);
                    if (i+j==8) flag[3][1] ^= (1u << k);
                    stk[i][j] = 0;
                }
                return false;
            }
        }
    }
    return true;
}

bool SudokuGame::ids(int stk[9][9],
                     uint16_t flag[4][9],
                     int nowdep, int maxdep, int &dep_reached)
{
    if (nowdep > maxdep) return false;
    if (nowdep > dep_reached) dep_reached = nowdep;
    int mi = -1, mj = -1, mid = -1; int mcnt = 0;
    bool finished = true;
    std::vector<std::pair<int, int> > changes_to_be_reverted;
    // iteratively check if we can uniquely determine some cells
    bool Iterate = true;
    while (Iterate) {
        Iterate = false;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (stk[i][j] != 0) continue;
                int id = (i/3) * 3 + j/3;
                uint16_t fff = flag[0][id] | flag[1][j] | flag[2][i];
                if (i == j) fff |= flag[3][0];
                if (i+j==8) fff |= flag[3][1];
                int cnt = count_bits(fff);
                if (cnt == 9) {
                // there is a contradiction: all numbers have appeared in the region,
                //   but stk[i][j] hasn't been filled in yet.
                    goto CLEANUP_AND_EXIT;
                }
                if (cnt != 8) continue;
                // this cell is uniquely determined, so we don't bother going
                //   into recursion, but fill it in directly.
                for (int k = 1; k <= 9; k++) {
                    if (!(fff & (1u << k))) {
                        stk[i][j] = k;
                        flag[0][id] ^= (1u << k);
                        flag[1][j]  ^= (1u << k);
                        flag[2][i]  ^= (1u << k);
                        if (i == j) flag[3][0] ^= (1u << k);
                        if (i+j==8) flag[3][1] ^= (1u << k);
                // changes made here need to be reverted if subsequent
                //   recursions should fail. hence we record the changes in an
                //   std::vector.
                        changes_to_be_reverted.push_back(std::make_pair(i, j));
                        break;
                    }
                }
                // after `flag` changes, potentially more cells become uniquely
                //   decidable, so we will need to iterate.
                Iterate = true;
            }
        }
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (stk[i][j] != 0) continue;
            finished = false;
            int id = (i/3) * 3 + j/3;
            uint16_t fff = flag[0][id] | flag[1][j] | flag[2][i];
            if (i == j) fff |= flag[3][0];
            if (i+j==8) fff |= flag[3][1];
            int cnt = count_bits(fff);
            if (cnt > mcnt) {
                mi = i, mj = j, mid = id, mcnt = cnt;
            }
        }
    }
    if (finished) { return true; }

    // now go into recursion
    for (int k = 1; k <= 9; k++) {
        if (!((flag[0][mid] | flag[1][mj] | flag[2][mi]) & (1u << k))) {
            flag[0][mid] ^= (1u << k);
            flag[1][mj]  ^= (1u << k);
            flag[2][mi]  ^= (1u << k);
            if (mi == mj) flag[3][0] ^= (1u << k);
            if (mi+mj==8) flag[3][1] ^= (1u << k);
            stk[mi][mj] = k;
            finished = ids(stk, flag, nowdep + 1, maxdep, dep_reached);
            if (finished) return true;
            flag[0][mid] ^= (1u << k);
            flag[1][mj]  ^= (1u << k);
            flag[2][mi]  ^= (1u << k);
            if (mi == mj) flag[3][0] ^= (1u << k);
            if (mi+mj==8) flag[3][1] ^= (1u << k);
            stk[mi][mj] = 0;
        }
    }

    // now revert all the changes made since we didn't succeed & return true
    // the `goto` statement is used here rationally, with no side-effect
CLEANUP_AND_EXIT:
    for (auto &&p : changes_to_be_reverted) {
        int i = p.first, j = p.second;
        int &k = stk[i][j];
        int id = (i/3) * 3 + j/3;
        flag[0][id] ^= (1u << k);
        flag[1][j]  ^= (1u << k);
        flag[2][i]  ^= (1u << k);
        if (i == j) flag[3][0] ^= (1u << k);
        if (i+j==8) flag[3][1] ^= (1u << k);
        k = 0;
    }
    return false;
}

int SudokuGame::count_bits(int t)
{
#define lowbit(t) ((t) & (-(t)))
    int cnt = 0;
    while (t) {
        t ^= lowbit(t);
        ++cnt;
    }
    return cnt;
#undef lowbit
}

bool SudokuGame::GetSolution(int (*p)[9][9], int MOD)
{
    // uniqueness not guaranteed
    // first calculate bit-flags in { blocks, columns, rows, diagonals }
    uint16_t flag[4][9];
    memset(flag, 0, sizeof(flag));
    int maxdep = 0;             // for ids
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!(*p)[i][j]) { ++maxdep; continue; }
            // blocks
            int id = (i/3) * 3 + j/3;
            if (flag[0][id] & (1u << (*p)[i][j])) { return false; }
            flag[0][id] |= (1u << (*p)[i][j]);
            // columns
            if (flag[1][j]  & (1u << (*p)[i][j])) { return false; }
            flag[1][j]  |= (1u << (*p)[i][j]);
            // rows
            if (flag[2][i]  & (1u << (*p)[i][j])) { return false; }
            flag[2][i]  |= (1u << (*p)[i][j]);
            // diagonals
            if (i == j) {
                if (flag[3][0] & (1u << (*p)[i][j])) { return false; }
                flag[3][0] |= (1u << (*p)[i][j]);
            }
            if (i+j==8) {
                if (flag[3][1] & (1u << (*p)[i][j])) { return false; }
                flag[3][1] |= (1u << (*p)[i][j]);
            }
        }
    }
    bool f = false;
    int reached = 0;
    switch (MOD) {
        case SudokuGame::STABLE_DFS:
            f = this->dfs(*p, flag, 0, -1);
            return f;
            break;
        case SudokuGame::DFS:
            return this->ids(*p, flag, 1, maxdep, reached);
            break;
        case SudokuGame::IDS:
            for (int k = std::min(maxdep, 7); k <= maxdep; k++, reached = 0) {
                if (this->ids(*p, flag, 1, k, reached))
                    return true;
                // when the IDS search doesn't go beyond maxdep=k,
                //   the sudoku has no solution
                if (reached != k)
                    return false;
            }
            return false;
            break;
        case SudokuGame::ASTAR:
            throw "not implemented";
            break;
        case SudokuGame::IDASTAR:
            throw "not implemented";
            break;
        case SudokuGame::DLX:
            throw "not implemented";
            break;
        case SudokuGame::PARALLEL_DFS:
            throw "not implemented";
            break;
    }
    return false;
}

void SudokuGame::Save(const char *filename, const int (*current)[9][9])
{
    // saves the reference board and current filled board
    bool given[9][9];
    std::memset(given, 0, sizeof(given));
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (gameBoard[i][j])
                given[i][j] = true;
    FILE *f = std::fopen(filename, "wb");
    std::fwrite(*current, sizeof(int), 81, f);
    std::fwrite(given, sizeof(bool), 81, f);
    std::fclose(f);
}

bool SudokuGame::Read(const char *filename, int (*current)[9][9])
{
    bool given[9][9];
    FILE *f = std::fopen(filename, "rb");
    if (f == NULL) return false;
    std::fread(*current, sizeof(int), 81, f);
    std::fread(given, sizeof(bool), 81, f);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (given[i][j])
                gameBoard[i][j] = (*current)[i][j];
            else
                gameBoard[i][j] = 0;
    return true;
}
