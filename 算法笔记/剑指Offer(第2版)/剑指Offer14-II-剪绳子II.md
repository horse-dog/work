## 剪绳子II

--------------------

- 给你一根长度为 `n` 的绳子，请把绳子剪成整数长度的 `m` 段（`m`、`n`都是整数，`n`>1并且`m`>1），每段绳子的长度记为 `k[0],k[1]...k[m-1]` 。请问 `k[0]*k[1]*...*k[m-1]` 可能的最大乘积是多少？例如，当绳子的长度是8时，我们把它剪成长度分别为2、3、3的三段，此时得到的最大乘积是18。

- 答案需要取模 1e9+7（1000000007），如计算初始结果为：1000000008，请返回 1。

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

    - 2 <= n <= 1000

--------------------

```cpp
class Solution {
public:
    int cuttingRope(int n) {
        if (n <= 3) return n - 1;
        int m = n / 3;
        int mod = 1e9 + 7;
        if (n % 3 == 0)
            return fastpowWithmod(3, m, mod);
        else if (n % 3 == 1)
            return (fastpowWithmod(3, m - 1, mod) * 4) % mod;
        else
            return (fastpowWithmod(3, m, mod) * 2) % mod;
    }

    size_t fastpowWithmod(size_t __b, size_t __e, size_t __m) {
        size_t __result = 1;
        while (__e != 0) {
            if (__e & 1)
                __result = (__result * __b) % __m;
            __b = (__b * __b) % __m;
            __e >>= 1;
        }
        return __result;
    }
};

```
--------------------

- 贪心法

- 定义绳长为n，每段绳子长度为$x_{i},i=0,1,\cdots ,m$，乘积为y，$x_{i}$相等时y取极大值。

- 二分法取模
```txt
 (a + b) % c = ((a % c) + (b % c)) % c
 (a * b) % c = ((a % c) * (b % c)) % c
 fn(s) = 3^s % m
 fn(s) = (3^(s/2) * 3^(s/2)) % m 
       = (3^(s/2) % m * 3^(s/2) % m) % m
       = (fn(s/2) * fn(s/2)) % m

 fn(s) = (3^(s/2) * 3^(s/2) * 3) % m 
       = ((3^(s/2) * 3^(s/2)) % m * (3 % m)) % m 
       = (3 * (3^(s/2) * 3^(s/2)) % m) % m
       = (3 * fn(s-1)) % m
**/
```

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