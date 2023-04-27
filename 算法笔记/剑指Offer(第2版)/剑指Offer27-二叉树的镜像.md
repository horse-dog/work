## 二叉树的镜像

--------------------

- 请完成一个函数，输入一个二叉树，该函数输出它的镜像。

- 例如输入：

    ```txt
         4
       /   \
      2     7
     / \   / \
    1   3 6   9
    ```

- 镜像输出：

    ```txt
         4
       /   \
      7     2
     / \   / \
    9   6 3   1
    ```

--------------------

- 示例 1：

    - 输入：root = [4,2,7,1,3,6,9]

    - 输出：[4,7,2,9,6,3,1]

--------------------

- 限制：

    - 0 <= 节点个数 <= 1000

--------------------

- 递归

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
    TreeNode* mirrorTree(TreeNode* root) {
        if (root == nullptr) 
            return nullptr;
        mirrorTree(root->left);
        mirrorTree(root->right);
        swap(root->left, root->right);
        return root;
    }
};
```