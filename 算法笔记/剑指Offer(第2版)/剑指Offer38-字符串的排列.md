## 字符串的排列

--------------------

- 输入一个字符串，打印出该字符串中字符的所有排列。

- 你可以以任意顺序返回这个字符串数组，但里面不能有重复元素。

--------------------

- 示例 1：
    
    - 输入：s = "abc"

    - 输出：["abc","acb","bac","bca","cab","cba"]

--------------------

- 限制：

    - 1 <= s 的长度 <= 8

--------------------

- 回溯法

    - 注意相同字符需要跳过，使用 set 保证

    - 回溯时直接修改 str，思路与直接插入排序类似（维持一个已回溯段）。

```cpp
class Solution {
private:
    vector<string> res;

public:
    vector<string> permutation(string s) {
        do_permute(s, 0);
        return res;
    }

    void do_permute(string& str, int x) {
        if (x == str.size() - 1) {
            res.push_back(str);
            return;
        }
        set<int> exist;
        for (size_t i = x; i < str.size(); i++) {
            if (exist.find(str[i]) != exist.end()) continue;
            exist.insert(str[i]);
            swap(str[x], str[i]);
            do_permute(str, x + 1);
            swap(str[x], str[i]);
        }
    }
};
```