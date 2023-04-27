## 正则表达式匹配

--------------------

- 请实现一个函数用来匹配包含'. '和'*'的正则表达式。模式中的字符'.'表示任意一个字符，而'*'表示它前面的字符可以出现任意次（含0次）。在本题中，匹配是指字符串的所有字符匹配整个模式。例如，字符串"aaa"与模式"a.a"和"ab*ac*a"匹配，但与"aa.a"和"ab*a"均不匹配。

--------------------

- 示例 1：

    - 输入:

        - s = "aa"

        - p = "a"

    - 输出: false

    - 解释: "a" 无法匹配 "aa" 整个字符串。

- 示例 2：

    - 输入:

        - s = "aa"

        - p = "a*"

    - 输出: true

    - 解释: 因为 '*' 代表可以匹配零个或多个前面的那一个元素, 在这里前面的元素就是 'a'。因此，字符串 "aa" 可被视为 'a' 重复了一次。

- 示例 3：

    - 输入:

        - s = "ab"

        - p = ".*"
    
    - 输出: true

    - 解释: ".*" 表示可匹配零个或多个（'*'）任意字符（'.'）。

- 示例 4：

    - 输入:

        - s = "aab"
    
        - p = "c*a*b"
    
    - 输出: true

    - 解释: 因为 '*' 表示零个或多个，这里 'c' 为 0 个, 'a' 被重复一次。因此可以匹配字符串 "aab"。

- 示例 5：

    - 输入:

        - s = "mississippi"

        - p = "mis\*is\*p\*."

    - 输出: false

--------------------

- 限制：

    - s 可能为空，且只包含从 a-z 的小写字母。
    
    - p 可能为空，且只包含从 a-z 的小写字母以及字符 . 和 *，无连续的 '*'。

--------------------

- 方法一：词法分析 + 回溯法。首先分析正则表达式串的词法生成 token 的链表，然后根据链表的各个 token 匹配待匹配的字符串，在 0～多字符匹配处进行回溯直至匹配成功，如果回溯到了待匹配字符串的末尾的 `'\0'` 都还没有匹配成功，则匹配彻底失败，向上层返回 false，否则向上层返回 true。最后如果 token 被全部匹配完，且字符串里的每个字符都被匹配完，则整个表达式匹配成功，否则匹配失败。此方法逻辑清晰，实现简单。

```cpp
class Solution {
 private:
  struct node {
    short node_type;  // 0: char; 1: dot; 2: *
    short value;
    node* next;
  };

 public:
  bool isMatch(string s, string p) {
    node* tok = tokenize(p.c_str());
    bool result = parse_regex(tok, s.c_str());
    while (tok != nullptr) {
      node* next = tok->next;
      delete tok;
      tok = next;
    }
    return result;
  }
  node* tokenize(const char* str) {
    node guard{0};
    node* cur = &guard;
    while (*str != '\0') {
      if (*(str + 1) == '*' && cur->node_type == 2 && *str == cur->value) {
        str += 2;
        continue;
      }
      node* nd = new node();
      nd->value = *str;
      nd->next = nullptr;
      cur->next = nd;
      cur = nd;
      if (*(str + 1) == '*') {
        nd->node_type = 2;
        str += 2;
      } else {
        nd->node_type = (*str == '.') ? 1 : 0;
        ++str;
      }
    }
    return guard.next;
  }

  bool parse_regex(node* tok, const char* str) {
    while (tok != nullptr) {
      if (tok->node_type == 0) {
        if (*str == tok->value) {
          ++str;
          tok = tok->next;
        } else {
          return false;
        }
      } else if (tok->node_type == 1) {
        if (*str != '\0') {
          ++str;
          tok = tok->next;
        } else {
          return false;
        }
      } else {
        while (true) {
          if(parse_regex(tok->next, str) == false) {
            if (*str == '\0') return false;
            if (tok->value == '.' || tok->value == *str)
              ++str;
            else 
              return false;
          } else {
            return true;
          }
        }
      }
    }
    return *str == '\0';
  }
};
```
--------------------

- 方法二：动态规划。[leetcode链接](https://leetcode.cn/problems/zheng-ze-biao-da-shi-pi-pei-lcof/solution/zheng-ze-biao-da-shi-pi-pei-by-leetcode-s3jgn/)