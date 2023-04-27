## 1至n整数中1出现的次数

--------------------

- 输入一个整数 n ，求1～n这n个整数的十进制表示中1出现的次数。

- 例如，输入12，1～12这些整数中包含1 的数字有1、10、11和12，1一共出现了5次。

--------------------

- 示例 1：

    - 输入：n = 12
    
    - 输出：5

- 示例 2：

    - 输入：n = 13
    
    - 输出：6

--------------------

- 限制：

    - 1 <= n < 2^31

--------------------

- 找规律：从低位到高位统计各位1出现的次数。

```cpp
class Solution {
public:
    int countDigitOne(int n) {
        long long count = 0, x = n, base = 1;
        while (x > 0) {
            int r = x % 10;
            x /= 10;
            if (r == 0) count += x * base;
            else if (r == 1) count += (x * base + (n % base) + 1);
            else count += (x + 1) * base;
            base *= 10;
        }
        return count;
    }
};
```