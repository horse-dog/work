## 矩阵中的路径

--------------------

- 给定一个 `m x n` 二维字符网格 `board` 和一个字符串单词 `word` 。如果 word 存在于网格中，返回 `true` ；否则，返回 `false` 。

    - 单词必须按照字母顺序，通过相邻的单元格内的字母构成，其中“相邻”单元格是那些水平相邻或垂直相邻的单元格。同一个单元格内的字母不允许被重复使用。

--------------------

- 示例 1：

    - 输入：`board = [["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], word = "ABCCED"`

    - 输出：true

- 示例 2:

    - 输入：`board = [["a","b"],["c","d"]], word = "abcd"`

    - 输出：false

--------------------

- 限制：

    - m == board.length

    - n = board[i].length

    - 1 <= m, n <= 6

    - 1 <= word.length <= 15

    - board 和 word 仅由大小写英文字母组成

--------------------

```cpp
class Solution {
public:
    bool exist(vector<vector<char>>& board, string word) {
        size_t m = board.size(), n = board[0].size();

        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++) {
                if (check_exist(board, word.c_str(), i, j))
                    return true;
            }
        return false;
    }

    bool check_exist(vector<vector<char>>& board, 
                     const char *str, int i, int j) {
        if (*str == '\0') return true;
        if (i < 0 || i >= board.size()) return false;
        if (j < 0 || j >= board[0].size()) return false;
        if (board[i][j] != *str) return false;
        char tmp = board[i][j];
        board[i][j] = '0';
        ++str;
        bool result = check_exist(board, str, i - 1, j)
            || check_exist(board, str, i + 1, j)
            || check_exist(board, str, i, j - 1)
            || check_exist(board, str, i, j + 1);
        board[i][j] = tmp;
        return result;
    }
};

```
--------------------

- 回溯法