## 二叉树的镜像

--------------------

- 请实现一个函数，用来判断一棵二叉树是不是对称的。如果一棵二叉树和它的镜像一样，那么它是对称的。

- 例如，二叉树 [1,2,2,3,4,4,3] 是对称的。

    ```txt
        1
       / \
      2   2
     / \ / \
    3  4 4  3
    ```

- 但是下面这个 [1,2,2,null,3,null,3] 则不是镜像对称的:

    ```txt
        1
       / \
      2   2
       \   \
       3    3
    ```

--------------------

- 示例 1：

    - 输入：root = [1,2,2,3,4,4,3]

    - 输出：true

- 示例 2：

    - 输入：root = [1,2,2,null,3,null,3]
    
    - 输出：false

--------------------

- 限制：

    - 0 <= 节点个数 <= 10000

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
    bool isSymmetric(TreeNode* root) {
        return __isSymmetric(root, root);
    }

    bool __isSymmetric(TreeNode* t1, TreeNode *t2) {
        if (t1 == nullptr && t2 == nullptr) return true;
        if (t1 == nullptr || t2 == nullptr) return false;
        if (t1->val != t2->val) return false;
        return __isSymmetric(t1->left, t2->right)
            && __isSymmetric(t1->right, t2->left);
    }
};
```