## 和为s的连续正数序列

--------------------

- 输入一个正整数 target ，输出所有和为 target 的连续正整数序列（至少含有两个数）。

- 序列内的数字由小到大排列，不同序列按照首个数字从小到大排列。

--------------------

- 示例 1：

    - 输入：target = 9

    - 输出：[[2,3,4],[4,5]]

- 示例 2：

    - 输入：target = 15

    - 输出：[[1,2,3,4,5],[4,5,6],[7,8]]

--------------------

- 限制：

    - 1 <= target <= 10^5

--------------------

- 根据等差数列求和公式算出所有可行解。

```cpp
class Solution {
public:
    vector<vector<int>> findContinuousSequence(int target) {
        vector<vector<int>> res;
        int n = 0, t = target;
        for (int m = 1; m * 2 < target; m++) {
            long long k = 2 * m - 1;
            long long d = sqrt(k * k + 8 * t);
            if (d * d != (k * k + 8 * t) || ((d - k) & 1)) continue;
            int n = (d - k) / 2;
            vector<int> tmp(n);
            for (int i = 0; i < n; i++)
                tmp[i] = m + i;
            res.push_back(tmp);
        }
        return res;
    }
};

// m, n
// target = (m + m + n - 1) * n / 2 = (2m + n - 1) * n / 2;
// k = 2m - 1 ==> target = (n + k) * n / 2
// ==> n^2 + kn - 2t = 0;
// ==> n = [sqrt(k^2 + 8t) - k] / 2
```