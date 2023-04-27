## 把数字翻译成字符串

--------------------

- 给定一个数字，我们按照如下规则把它翻译为字符串：0 翻译成 “a” ，1 翻译成 “b”，……，11 翻译成 “l”，……，25 翻译成 “z”。一个数字可能有多个翻译。请编程实现一个函数，用来计算一个数字有多少种不同的翻译方法。

--------------------

- 示例 1：

    - 输入: 12258
    
    - 输出: 5

    - 解释: 12258有5种不同的翻译，分别是"bccfi", "bwfi", "bczi", "mcfi"和"mzi"

--------------------

- 限制：

    - 0 <= num < 2^31

--------------------

- 由低位向高位回溯

```cpp
class Solution {
public:
    int translateNum(int num) {
        if (num < 10) return 1;
        int cur = num % 10;
        int cases = 0;
        cases += translateNum(num / 10);
        if ((num % 100) < 26 && (num % 100) > 9) 
            cases += translateNum(num / 100);    
        return cases;
    }
};
```