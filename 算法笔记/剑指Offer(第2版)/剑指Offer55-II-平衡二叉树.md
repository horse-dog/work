## 平衡二叉树

--------------------

- 输入一棵二叉树的根节点，判断该树是不是平衡二叉树。如果某二叉树中任意节点的左右子树的深度相差不超过1，那么它就是一棵平衡二叉树。

--------------------

- 示例 1:

    - 给定二叉树 [3,9,20,null,null,15,7]

    ```txt
        3
       / \
      9  20
        /  \
       15   7
    ```

    - 返回 true 。

- 示例 2:

    - 给定二叉树 [1,2,2,3,3,null,null,4,4]

    ```txt
           1
          / \
         2   2
        / \
       3   3
      / \
     4   4
    ```

    - 返回 false 。

--------------------

- 限制：

    - 0 <= 树的结点个数 <= 10000

--------------------

- 自底向上递归，即一边计算高度一边判断当前子树是否为 avl。

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
    bool isBalanced(TreeNode* root) {
        return height(root) != -1;
    }
    int height(TreeNode* root) {
        if (root == nullptr) return 0;
        int hl = height(root->left), hr = height(root->right);
        if (hl == -1 || hr == -1 || abs(hl - hr) > 1) 
            return -1;
        return max(hl, hr) + 1;
    }
};
```