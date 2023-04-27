## 最长不含重复字符的子字符串

--------------------

- 请从字符串中找出一个最长的不包含重复字符的子字符串，计算该最长子字符串的长度。

--------------------

- 示例 1：

    - 输入: "abcabcbb"

    - 输出: 3 

    - 解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。

- 示例 2:

    - 输入: "bbbbb"

    - 输出: 1

    - 解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。

- 示例 3:

    - 输入: "pwwkew"

    - 输出: 3

    - 解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
    
- 请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。

--------------------

- 限制：

    - s.length <= 40000

--------------------

- 使用哈希表记录已经遍历过的字符，最多只会有26个字符被遍历（因为英文字母只有28个），因此可以使用长度为26的数组代替哈希表，简化为采用位图，则按8字节向上对齐为32bit。

```cpp
class Solution {
private:
    int tag[4] = {0};
    void set(size_t idx) {
        int i = idx / 32;
        int r = idx % 32;
        tag[i] |= (1 << r);
    }
    void clear(size_t idx) {
        int i = idx / 32;
        int r = idx % 32;
        tag[i] ^= (1 << r);  
    }
    bool has_set(size_t idx) {
        int i = idx / 32;
        int r = idx % 32;
        return tag[i] & (1 << r);
    }
public:
    int lengthOfLongestSubstring(string s) {
        int maxlen = 0, begin = 0, end = 0;
        for (int i = 0; i < s.size(); ++i) {
            while (has_set(s[i]))
                clear(s[begin++]);
            set(s[i]);
            ++end;
            if ((end - begin) > maxlen)
                maxlen = end - begin;
        }
        return maxlen;
    }
};
```