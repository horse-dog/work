## 树的子结构

--------------------

- 输入两棵二叉树A和B，判断B是不是A的子结构。(约定空树不是任意一个树的子结构)

- B是A的子结构， 即 A中有出现和B相同的结构和节点值。

- 例如:

- 给定的树 A:

    ```txt
         3
        / \
       4   5
      / \
     1   2
    ```

- 给定的树 B：

    ```txt
       4 
      /
     1
    ```

- 返回 true，因为 B 与 A 的一个子树拥有相同的结构和节点值。

--------------------

- 示例 1：

    - 输入：A = [1,2,3], B = [3,1]

    - 输出：false

- 示例 2：

    - 输入：A = [3,4,5,1,2], B = [4,1]

    - 输出：true

--------------------

- 限制：

    - 0 <= 节点个数 <= 10000

--------------------

- 递归判断：

    - B是A的子结构 <==> B是A的根相等的子结构 || B是A的左子树的子结构 || B是A的右子树的子结构

    - 注意，B为null时，B是任意树的根相等的子结构

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
    bool isSubStructure(TreeNode* A, TreeNode* B) {
        if (A == nullptr || B == nullptr) return false;
        return rootSub(A, B) || isSubStructure(A->left, B) || isSubStructure(A->right, B);
    }
    bool rootSub(TreeNode* A, TreeNode* B) {
        if (B == nullptr) return true;
        if (A == nullptr) return false;
        return (A->val == B->val)
            && rootSub(A->left, B->left) && rootSub(A->right, B->right);
    }
};
```