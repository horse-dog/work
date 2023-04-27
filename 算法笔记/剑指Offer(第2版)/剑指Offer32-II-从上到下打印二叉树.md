## 从上到下打印二叉树 II

--------------------

- 从上到下按层打印二叉树，同一层的节点按从左到右的顺序打印，每一层打印到一行。

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
      [9,20],
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
        while (!qu.empty()) {
            auto len = qu.size();
            vector<int> line;
            line.reserve(len);
            for (size_t i = 0; i < len; i++) {
                auto p = qu.front();
                if (p->left) qu.push(p->left);
                if (p->right) qu.push(p->right);
                line.push_back(p->val);
                qu.pop();
            }
            res.push_back(line);
        }
        return res;
    }
};
```