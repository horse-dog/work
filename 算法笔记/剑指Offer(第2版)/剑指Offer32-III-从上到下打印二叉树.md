## 从上到下打印二叉树 III

--------------------

- 请实现一个函数按照之字形顺序打印二叉树，即第一行按照从左到右的顺序打印，第二层按照从右到左的顺序打印，第三行再按照从左到右的顺序打印，其他行以此类推。

--------------------

- 例如:
    - 给定二叉树: [3,9,20,null,null,15,7],
    ```txt
         3
        / \
       9  20
         /  \
        15   7
    ```
    - 返回：
    ```txt
    [
      [3],
      [20,9],
      [15,7]
    ]
    ```
--------------------

- 限制：

    - 节点总数 <= 1000

--------------------

- 二叉树层次遍历，基础题

```cpp
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        if (root == nullptr) return {};
        queue<TreeNode*> qu;
        qu.push(root);
        vector<vector<int>> res;
        bool flag = true;
        while (!qu.empty()) {
            auto len = qu.size();
            vector<int> line(len);
            auto it = line.begin();
            auto rit = line.rbegin();
            for (size_t i = 0; i < len; i++) {
                auto p = qu.front();
                if (p->left) qu.push(p->left);
                if (p->right) qu.push(p->right);
                if (flag)
                    *it++ = p->val;
                else 
                    *rit++ = p->val;
                qu.pop();
            }
            flag = !flag;
            res.push_back(line);
        }
        return res;
    }
};
```