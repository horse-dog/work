## 打印从1到最大的n位数

--------------------

- 输入数字 `n`，按顺序打印出从 1 到最大的 n 位十进制数。比如输入 3，则打印出 1、2、3 一直到最大的 3 位数 999。

--------------------

- 示例 1：
    
    - 输入: n = 1
    
    - 输出: [1,2,3,4,5,6,7,8,9]

--------------------

- 限制：

    - 用返回一个整数列表来代替打印
    - `n` 为正整数

--------------------

- 方法一：直接 generate.

```cpp
class Solution {
public:
    vector<int> printNumbers(int n) {
        int num = pow(10, n) - 1;
        vector<int> res(num);
        int i = 0;
        generate(res.begin(), res.end(), [&i] {
            return ++i;
        });
        return res;
    }
};
```
--------------------

- 方法二：对于大数，采用回溯法

```cpp
class Solution {
    vector<string> res;
public:
    vector<string> printNumbers(int n) {
        res.clear();
        char* num = new char[n + 1];
        for (int i = 1; i <= n; i++) {
            num[i] = '\0';
            for (int j = 1; j <= 9; j++) {
                num[0] = '0' + j;
                backTrace(1, i, num);
            }
        }
        delete[] num;
        return res;
    }

    void backTrace(int cur, int bit, char* num) {
        if (cur == bit) {
            res.push_back(num);
            return;
        }
        for (int i = 0; i <= 9; i++) {
            num[cur] = '0' + i;
            backTrace(cur + 1, bit, num);
        }
    }
};
```