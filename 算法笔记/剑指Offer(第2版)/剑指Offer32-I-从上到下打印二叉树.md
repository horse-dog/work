## 从上到下打印二叉树 I

--------------------

- 从上到下打印出二叉树的每个节点，同一层的节点按照从左到右的顺序打印。

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
    - 返回：[3,9,20,15,7]

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
    vector<int> levelOrder(TreeNode* root) {
        if (root == nullptr) return {};
        queue<TreeNode*> qu;
        vector<int> res;
        qu.push(root);
        while (!qu.empty()) {
            auto nd = qu.front();
            qu.pop();
            res.push_back(nd->val);
            if (nd->left != nullptr)
                qu.push(nd->left);
            if (nd->right != nullptr)
                qu.push(nd->right);
        }
        return res;
    }
};
```