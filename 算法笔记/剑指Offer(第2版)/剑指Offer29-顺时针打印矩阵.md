## 顺时针打印矩阵

--------------------

- 输入一个矩阵，按照从外向里以顺时针的顺序依次打印出每一个数字。

--------------------

- 示例 1：

    - 输入：matrix = [[1,2,3],[4,5,6],[7,8,9]]

    - 输出：[1,2,3,6,9,8,7,4,5]

- 示例 2：

    - 输入：matrix = [[1,2,3,4],[5,6,7,8],[9,10,11,12]]

    - 输出：[1,2,3,4,8,12,11,10,9,5,6,7]

--------------------

- 限制：

    - 0 <= matrix.length <= 100
    
    - 0 <= matrix[i].length <= 100

--------------------

- 循环打印

```cpp
class Solution  {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        if (matrix.empty()) return {};
        int l = 0;                      // 左边界
        int r = matrix[0].size() - 1;   // 右边界
        int t = 0;                      // 上边界
        int b = matrix.size() - 1;      // 下边界
        int cur = 0;
        vector<int> res((r + 1) * (b + 1));
        while (true) {
            // left -> right
            for (int i = l; i <= r; i++) res[cur++] = matrix[t][i];
            if (++t > b) break;
            // top -> bottom
            for (int i = t; i <= b; i++) res[cur++] = matrix[i][r];
            if (--r < l) break;
            // right -> left
            for (int i = r; i >= l; i--) res[cur++] = matrix[b][i];
            if (--b < t) break;
            // bottom -> top
            for (int i = b; i >= t; i--) res[cur++] = matrix[i][l];
            if (++l > r) break;
        }
        return res;
    }
};
```