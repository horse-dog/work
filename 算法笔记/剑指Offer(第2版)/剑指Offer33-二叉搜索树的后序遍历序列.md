## 二叉搜索树的后序遍历序列

--------------------

- 输入一个整数数组，判断该数组是不是某二叉搜索树的后序遍历结果。如果是则返回 true，否则返回 false。假设输入的数组的任意两个数字都互不相同。

--------------------

- 参考以下这颗二叉搜索树
    ```txt
         5
        / \
       2   6
     /  \
    1    3
    ```

- 示例 1：

    - 输入: [1,6,3,2,5]

    - 输出: false

- 示例 2：

    - 输入: [1,3,2,6,5]

    - 输出: true

--------------------

- 限制：

    - 数组长度 <= 1000

--------------------

- 递归判断

```cpp
class Solution {
public:
    bool verifyPostorder(vector<int>& postorder) {
        return verify(postorder, 0, postorder.size());
    }
    bool verify(const vector<int>& dict, int from, int to) {
        if (from == to) return true;
        int root = dict[to - 1];
        int split = from;
        for (; (split < to - 1) && dict[split] < root; ++split);
        for (int ck = split; ck < to - 1; ck++)
            if (dict[ck] < root) return false;
        return verify(dict, from, split)
            && verify(dict, split, to - 1);
    }
};
```