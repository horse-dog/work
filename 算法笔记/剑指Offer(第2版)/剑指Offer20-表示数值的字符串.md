## 表示数值的字符串

--------------------

- 请实现一个函数用来判断字符串是否表示数值（包括整数和小数）。

- 数值（按顺序）可以分成以下几个部分：

    - 1.若干空格
    
    - 2.一个 小数 或者 整数
    
    - 3.(可选)一个 'e' 或 'E' ，后面跟着一个 整数
    
    - 4.若干空格

- 小数（按顺序）可以分成以下几个部分：

    - 1.(可选)一个符号字符（'+' 或 '-'）
    
    - 2.下述格式之一：

        - 1.至少一位数字，后面跟着一个点 '.'
    
        - 2.至少一位数字，后面跟着一个点 '.' ，后面再跟着至少一位数字
    
        - 3.一个点 '.' ，后面跟着至少一位数字

- 整数（按顺序）可以分成以下几个部分：

    - 1.(可选)一个符号字符（'+' 或 '-'）

    - 2.至少一位数字

- 部分数值列举如下：

    - ["+100", "5e2", "-123", "3.1416", "-1E-16", "0123"]

- 部分非数值列举如下：

    - ["12e", "1a3.14", "1.2.3", "+-5", "12e+5.4"]

--------------------

- 示例 1：

    - 输入：s = "0"

    - 输出：true

- 示例 2：

    - 输入：s = "e"
    
    - 输出：false

- 示例 3：

    - 输入：s = "."
    
    - 输出：false

--------------------

- 限制：

    - 1 <= s.length <= 20
    
    - s 仅含英文字母（大写和小写），数字（0-9），加号 '+' ，减号 '-' ，空格 ' ' 或者点 '.' 。

--------------------

- 方法一：词法分析(庖丁解牛).

```cpp
class Solution {
public:
    bool isNumber(string s) {
        const char* context = s.c_str();
        return is_arithmic(context);
    }

    // arithmic = {space}* + {numeric} + {exp}? + {space}* + '\0'
    bool is_arithmic(const char* ctx) {
        skip_space(ctx);
        if (parse_numeric(ctx) == false) 
            return false;
        if (*ctx == 'e' || *ctx == 'E')
            if (parse_exp(ctx) == false) 
                return false;
        skip_space(ctx);
        return *ctx == '\0';
    }

    void skip_space(const char*& ctx) {
        while (*ctx == ' ')
            ++ctx;
    }

    // numeric = {decimal} | {integer}
    bool parse_numeric(const char*& ctx) {
        const char* pre_ctx = ctx;
        if (parse_decimal(ctx))
            return true;
        ctx = pre_ctx;
        return parse_integer(ctx);
    }

    // integer = ('+' | '-')? + {number}
    bool parse_integer(const char*& ctx) {
        if (*ctx == '+' || *ctx == '-')
            ++ctx;
        return parse_number(ctx);
    }

    // decimal = ('+' | '-')? + (.{number} | {number}.{number} | {number.})
    bool parse_decimal(const char*& ctx) {
        if (*ctx == '+' || *ctx == '-')
            ++ctx;
        if (*ctx == '.') {
            // '.' + number
            ++ctx;
            return parse_number(ctx);
        } else {
            if (parse_number(ctx) == false)
                return false;
            if (*ctx != '.')
                return false;
            ++ctx;
            const char* pre_ctx = ctx;
            if (parse_number(ctx) == false)
                ctx = pre_ctx;
            return true;
        }
    }

    // number = '[0~9]'+
    bool parse_number(const char*& ctx) {
        if (*ctx < '0' || *ctx > '9')
            return false;
        while (*ctx >= '0' && *ctx <= '9')
            ++ctx;
        return true;
    }

    // exp = ('e' | 'E') + {integer}
    bool parse_exp(const char*& ctx) {
        if (*ctx != 'e' && *ctx != 'E')
            return false;
        ++ctx;
        return parse_integer(ctx);
    }
};
```
--------------------

- 方法二：有限状态机。[leetcode链接](https://leetcode.cn/problems/biao-shi-shu-zhi-de-zi-fu-chuan-lcof/solution/biao-shi-shu-zhi-de-zi-fu-chuan-by-leetcode-soluti/)