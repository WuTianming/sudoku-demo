#include "sudokugame.h"
#include "sudokunode.h"

// the constructor in turn calls the constructor of the random generator.
SudokuGame::SudokuGame() : rand(time(NULL)) {}
SudokuGame::SudokuGame(unsigned seed) : rand(seed) {}
// SudokuGame::SudokuGame() : rand(QDateTime::currentMSecsSinceEpoch())

// try to generate valid filled Sudoku puzzles using rand17 & solve method
bool SudokuGame::GenSolvable_rand17(int s[9][9], int dep = 9)
{
    uint16_t flag[4][9];
    memset(s, 0, sizeof(int) * 81);
    memset(flag, 0, sizeof(flag));
    for (int T = 1; T <= 17; T++) {
        int i = rand() % 9, j = rand() % 9;
        while (s[i][j]) i = rand()%9, j = rand()%9;
        uint16_t fff = get_flag(flag, i, j);
        int cnt = count_bits(fff);
        if (cnt == 9) return false;
        int it = rand() % (9 - cnt) + 1;
        for (int k = 1; k <= 9; k++) {
            // count until the it-th vacant number
            if (!(fff & (1u << k))) it--;
            if (!it) {
                // then fill in
                set_flag(flag, i, j, k);
                s[i][j] = k;
                break;
            }
        }
    }
    int reached = 0;
    // return astar(s);
    for (int k = std::min(dep, 4); k <= dep; k++, reached = 0) {
        if (ids(s, flag, 1, k, reached)) return true;
        if (reached != k) return false;
    }
    return false;
}

// generate valid filled Sudoku puzzles
void SudokuGame::GeneratePuzzle()
{
    memset(gameBoard, 0, sizeof(gameBoard));
    int i = 0;
    static int per[] = { 7, 9, 14 };
    while (!GenSolvable_rand17(gameBoard, per[i])) { (++i) %= 3; }
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

bool SudokuGame::verify(const int ans[9][9])
{
    int cnt[10], tot = 0;
    memset(cnt, 0, sizeof(cnt)); cnt[0] = -10000;
    ++tot;
#ifdef XSUDOKU
    // check two diagonals
    for (int i = 0; i < 9; i++)
        if (cnt[ans[i][i]] != tot - 1) return false;
        else cnt[ans[i][i]] = tot;
    ++tot;
    for (int i = 0; i < 9; i++)
        if (cnt[ans[i][8-i]] != tot - 1) return false;
        else cnt[ans[i][8-i]] = tot;
    ++tot;
#endif
    // check every row
    for (int i = 0; i < 9; i++, tot++)
        for (int j = 0; j < 9; j++)
            if (cnt[ans[i][j]] != tot - 1) return false;
            else cnt[ans[i][j]] = tot;
    // check every column
    for (int j = 0; j < 9; j++, tot++)
        for (int i = 0; i < 9; i++)
            if (cnt[ans[i][j]] != tot - 1) return false;
            else cnt[ans[i][j]] = tot;
    // check every block
    for (int i = 0; i < 9; i += 3)
        for (int j = 0; j < 9; j += 3, tot++)
            for (int k = i; k < i + 3; k++)
                for (int l = j; l < j + 3; l++)
                    if (cnt[ans[k][l]] != tot - 1) return false;
                    else cnt[ans[k][l]] = tot;
    return true;
}

bool SudokuGame::isValid()
{
    return verify(this->gameBoard);
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
                uint16_t fff = get_flag(flag, i, j);
                for (int k = 1; k <= 9; k++) {
                    if (fff & (1u << k)) continue;
                    set_flag(flag, i, j, k);
                    stk[i][j] = k;
                    bool f = dfs(stk, flag, i, j);
                    if (f) { return true; }
                    set_flag(flag, i, j, k);
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
    int mi = -1, mj = -1; int mcnt = 0;
    bool finished = true;
    std::vector<std::pair<int, int> > changes_to_be_reverted;
    // changes_to_be_reverted.reserve(80);
    // iteratively check if we can uniquely determine some cells
    bool Iterate = true;
    while (Iterate) {
        Iterate = false;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (stk[i][j] != 0) continue;
                uint16_t fff = get_flag(flag, i, j);
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
                        set_flag(flag, i, j, k);
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
            uint16_t fff = get_flag(flag, i, j);
            int cnt = count_bits(fff);
            if (cnt > mcnt) {
                mi = i, mj = j, mcnt = cnt;
            }
            // if (mcnt == 7) break;
        }
    }
    if (finished) { return true; }

    // now go into recursion
    do {
        uint16_t fff = get_flag(flag, mi, mj);
        for (int k = 1; k <= 9; k++) {
            if (!(fff & (1u << k))) {
                set_flag(flag, mi, mj, k);
                stk[mi][mj] = k;
                finished = ids(stk, flag, nowdep + 1, maxdep, dep_reached);
                if (finished) return true;
                set_flag(flag, mi, mj, k);
                stk[mi][mj] = 0;
            }
        }
    } while(false);

    // now revert all the changes made since we didn't succeed
    // the `goto` statement is used here rationally, with no side-effect
CLEANUP_AND_EXIT:
    for (auto &&p : changes_to_be_reverted) {
        int &i = p.first, &j = p.second;
        set_flag(flag, i, j, stk[i][j]);
        stk[i][j] = 0;
    }
    return false;
}

// TODO
bool SudokuGame::astar(int stk[9][9])
{
    std::priority_queue<SudokuNode> Q;
    Q.emplace(stk);
    while (!Q.empty()) {
        SudokuNode p = Q.top();
        Q.pop();
        // find neighbour
        int mi, mj, mcnt = 0;
        bool finished = true;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (p.a[i][j] != 0) continue;
                finished = false;
                int cnt = count_bits(get_flag(p.flag, i, j));
                if (cnt > mcnt) { mi = i, mj = j, mcnt = cnt; }
                if (mcnt == 8) break;
            }
        }
        if (finished) {
            // p is complete
            memcpy(stk, p.a, sizeof(int) * 81);
            return true;
        }
        uint16_t fff = get_flag(p.flag, mi, mj);
        for (int k = 1; k <= 9; k++) {
            if (!(fff & (1u << k))) {
                set_flag(p.flag, mi, mj, k);
                p.a[mi][mj] = k;
                p.cal_cost() += 9 - mcnt;
                Q.push(p);
                set_flag(p.flag, mi, mj, k);
                p.a[mi][mj] = 0;
            }
        }
    }
    return false;
}

bool SudokuGame::cal_flag(const int s[9][9], uint16_t flag[4][9])
{
    memset(flag, 0, sizeof(uint16_t) * 36);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!s[i][j]) continue;
            if (get_flag(flag, i, j) & (1u << s[i][j])) { return false; }
            set_flag(flag, i, j, s[i][j]);
        }
    }
    return true;
}

bool SudokuGame::GetSolution(int (*p)[9][9], int MOD)
{
    // uniqueness not guaranteed
    uint16_t flag[4][9];
    // memset(flag, 0, sizeof(flag));
    // first calculate bit-flags in { blocks, columns, rows, diagonals }
    int maxdep = 0;             // for ids
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            maxdep += !(*p)[i][j];
    int reached = 0;
    switch (MOD) {
        case SudokuGame::STABLE_DFS:
            cal_flag(*p, flag);
            return dfs(*p, flag, 0, -1);
            break;
        case SudokuGame::DFS:
            cal_flag(*p, flag);
            return ids(*p, flag, 1, 81, reached);
            break;
        case SudokuGame::IDS:
            cal_flag(*p, flag);
            for (int k = std::min(maxdep, 5); k <= maxdep; k++, reached = 0) {
                if (ids(*p, flag, 1, k, reached))
                    return true;
                // when the IDS search doesn't go beyond maxdep=k,
                //   the sudoku has no solution
                if (reached != k)
                    return false;
            }
            return false;
            break;
        case SudokuGame::ASTAR:
            return astar(*p);
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

// TODO 将当前的数独状态，记录名称，当前时间保存到同一目录的文件下
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
    std::fclose(f);
    return true;
}
