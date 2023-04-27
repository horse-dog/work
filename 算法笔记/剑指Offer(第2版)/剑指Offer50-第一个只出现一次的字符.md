## 第一个只出现一次的字符

--------------------

- 在字符串 s 中找出第一个只出现一次的字符。如果没有，返回一个单空格。 s 只包含小写字母。

--------------------

- 示例 1：

    - 输入：s = "abaccdeff"

    - 输出：'b'

- 示例 2:

    - 输入：s = "" 

    - 输出：' '

--------------------

- 限制：

    - 0 <= s 的长度 <= 50000

--------------------

- 统计26个小写字母出现的频率即可。

```cpp
class Solution {
public:
    char firstUniqChar(string s) {
        int char_cnt[26] {};
        for (auto&& c : s)
            ++char_cnt[c - 'a'];
        for (auto&& c : s)
            if (char_cnt[c - 'a'] == 1)
                return c;
        return ' ';
    }
};
```