## 二维数组中的查找

--------------------

- 在一个 n * m 的二维数组中，每一行都按照从左到右 非递减 的顺序排序，每一列都按照从上到下 非递减 的顺序排序。请完成一个高效的函数，输入这样的一个二维数组和一个整数，判断数组中是否含有该整数。

--------------------

- 示例 1：

    - 现有矩阵 matrix 如下：
    
    ```text
    [
      [1,   4,  7, 11, 15],
      [2,   5,  8, 12, 19],
      [3,   6,  9, 16, 22],
      [10, 13, 14, 17, 24],
      [18, 21, 23, 26, 30]
    ]
    ```

    - 给定 target = 5，返回 true。
    
    - 给定 target = 20，返回 false。

--------------------

- 限制：

    - 0 <= n <= 1000

    - 0 <= m <= 1000

--------------------

```cpp
class Solution {
public:
    bool findNumberIn2DArray(vector<vector<int>>& matrix, int target) {
        int m = matrix.size();
        if (m == 0) return false;
        int n = matrix[0].size();
        if (n == 0) return false;
        int x = 0, y = n - 1;
        while (x < m && y >= 0) {
            if (matrix[x][y] == target) return true;
            if (matrix[x][y] > target) --y;
            else ++x;
        }
        return false;
    }
};
```
--------------------

- 如果 matrix[x, y] > target, 则 matrix[x:, y] > target 恒成立

- 如果 matrix[x, y] < target, 则 matrix[x, :y] < target 恒成立