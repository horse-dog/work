## I. 斐波拉契数列

--------------------

- 写一个函数，输入 n ，求斐波那契（Fibonacci）数列的第 n 项（即 F(N)）。斐波那契数列的定义如下:
    - F(0) = 0,   F(1) = 1
    - F(N) = F(N - 1) + F(N - 2), 其中 N > 1.
    - 斐波那契数列由 0 和 1 开始，之后的斐波那契数就是由之前的两数相加而得出。
    - 答案需要取模 1e9+7（1000000007），如计算初始结果为：1000000008，请返回 1。

--------------------

- 示例 1：

    - 输入：n = 2

    - 输出：1

- 示例 2:

    - 输入：n = 5

    - 输出：5

--------------------

- 限制：

    - 0 <= n <= 100

--------------------

```cpp
class Solution {
public:
    int fib(int n) {
        if (n < 2) return n;
        const int mod = 1e9 + 7;
        int a = 0, b = 1, c;
        for (int i = 1; i < n; i++) {
            c = (a + b) % mod;
            a = b, b = c;
        }
        return c;
    }
};
```
--------------------

- 模运算规则：
    - (a + b) % c = [(a % c) + (b % c)] % c
    - (a - b) % c = [(a % c) - (b % c)] % c
    - (a * b) % c = [(a % c) * (b % c)] % c
    - (a ^ b) % c = [(a % c) ^ b] % c

- 方法一: 动态规划，O(n)
- 方法二: 快速幂，O(logn)

$$
F_{n} =
\begin{cases}
n, n < 2 \\
F_{n-1} + F_{n-2}, n >= 2
\end{cases}
$$

$$
\begin{pmatrix}1 & 1 \\ 1 & 0\\ 
\end{pmatrix}
\begin{pmatrix}F_{n-1} \\ F_{n-2}\\ 
\end{pmatrix} =
\begin{pmatrix}F_{n} \\ F_{n-1}\\ 
\end{pmatrix}
$$

$$
\begin{pmatrix}F_{n} \\ F_{n-1}\\ 
\end{pmatrix} = 
\begin{pmatrix}1 & 1 \\ 1 & 0\\ 
\end{pmatrix} ^ {n - 1}
\begin{pmatrix}F_{1} \\ F_{0}\\ 
\end{pmatrix}
$$

$$
\begin{pmatrix}1 & 1 \\ 1 & 0\\ 
\end{pmatrix} ^ {n - 1} 计算采用快速幂
$$

```cpp
class Solution {

class array2x2_t {
private:
    long m_data[4];
    const static int MOD = 1e9 + 7;
public:
    array2x2_t(int a, int b, int c, int d) {
        m_data[0] = a, m_data[1] = b;
        m_data[2] = c, m_data[3] = d;
    }
    int first() const 
    { return static_cast<int>(m_data[0]); }
    array2x2_t &operator*=(const array2x2_t& __x) noexcept {
        long a, b, c, d;
        a = (m_data[0] * __x.m_data[0] 
           + m_data[1] * __x.m_data[2]) % array2x2_t::MOD;
        b = (m_data[0] * __x.m_data[1] 
           + m_data[1] * __x.m_data[3]) % array2x2_t::MOD;
        c = (m_data[2] * __x.m_data[0] 
           + m_data[3] * __x.m_data[2]) % array2x2_t::MOD;
        d = (m_data[2] * __x.m_data[1] 
           + m_data[3] * __x.m_data[3]) % array2x2_t::MOD;
        m_data[0] = a, m_data[1] = b;
        m_data[2] = c, m_data[3] = d;
        return *this;
    }
};

public:
    int fib(int n) {
        if (n < 2) return n;
        n -= 1;
        array2x2_t R(1, 0, 0, 1), X(1, 1, 1, 0);
        while (n != 0) {
            if (n & 1)
                R *= X;
            X *= X;
            n >>= 1;
        }
        return R.first();
    }
};
```
