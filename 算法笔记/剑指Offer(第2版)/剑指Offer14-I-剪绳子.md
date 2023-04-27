## 剪绳子

--------------------

- 给你一根长度为 `n` 的绳子，请把绳子剪成整数长度的 `m` 段（`m`、`n`都是整数，`n`>1并且`m`>1），每段绳子的长度记为 `k[0],k[1]...k[m-1]` 。请问 `k[0]*k[1]*...*k[m-1]` 可能的最大乘积是多少？例如，当绳子的长度是8时，我们把它剪成长度分别为2、3、3的三段，此时得到的最大乘积是18。

--------------------

- 示例 1：
    
    - 输入: 2

    - 输出: 1

    - 解释: 2 = 1 + 1, 1 × 1 = 1

- 示例 2:

    - 输入: 10

    - 输出: 36

    - 解释: 10 = 3 + 3 + 4, 3 × 3 × 4 = 36

--------------------

- 限制：

    - 2 <= n <= 58

--------------------

```cpp
class Solution {
public:
    int cuttingRope(int n) {
        if (n <= 3) return n - 1;
        int m = n / 3;
        if (n % 3 == 0)
            return fastpow(3, m);
        else if (n % 3 == 1)
            return fastpow(3, m - 1) * 4;
        else
            return fastpow(3, m) * 2;
    }

    int fastpow(size_t __b, size_t __e) {
        int __result = 1;
        while (__e != 0) {
            if (__e & 1)
                __result *= __b;
            __b *= __b;
            __e >>= 1;
        }
        return __result;
    }
};

```
--------------------

- 贪心法

- 定义绳长为n，每段绳子长度为$x_{i},i=0,1,\cdots ,m$，乘积为y，$x_{i}$相等时y取极大值。

- 令 $x_{0}=x_{1}=\cdots=x$:

$$
y=x^{\frac{n}{x}}
$$

$$
lny=\frac{n}{x} \ast lnx=n \ast \frac{lnx}{x}
$$

$$
\frac{1}{y} \ast \frac{dy}{dx}=n \ast \frac{1-lnx}{x^2}
$$

$$
\frac{dy}{dx}\sim \frac{1-lnx}{x^2}
$$

$$
y_{max}=y|_{x=e}
$$

- x = e 时，乘积取最大值。

- x 为整数，则 x = 2 或 x = 3 时 y 取最大值，带入方程得 x = 3 时 y 取最大值。

- 推导结果为：

$$
y=\begin{cases}
n-1, n <= 3 \\
3^{n/3}, n > 3 且 n \pmod 3=0 \\
3^{(n-4)/3} \ast 2 \ast 2, n > 3 且 n \pmod 3=1 \\
3^{(n-2)/3} \ast 2, n > 3 且 n \pmod 3=2 \\
\end{cases}
$$