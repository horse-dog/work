## 二叉搜索树的第K大节点

--------------------

- 给定一棵二叉搜索树，请找出其中第 k 大的节点的值。

--------------------

- 示例 1:

    - 输入: root = [3,1,4,null,2], k = 1
    
    ```txt
       3
      / \
     1   4
      \
       2
    ```

    - 输出: 4

- 示例 2:

    - 输入: root = [5,3,6,2,4,null,null,1], k = 3

    ```txt
           5
          / \
         3   6
        / \
       2   4
      /
     1
    ```

    - 输出: 4

--------------------

- 限制：

    - 1 ≤ k ≤ 二叉搜索树元素个数

--------------------

- 中序遍历，先右后左

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
    int kthLargest(TreeNode* root, int k) {
        stack<TreeNode*> st;
        TreeNode* p = root;
        while (!st.empty() || p != nullptr) {
            while (p != nullptr) {
                st.push(p);
                p = p->right;
            }
            p = st.top();
            st.pop();
            if (--k == 0) return p->val;
            p = p->left;
        }
        return 0;
    }
};
```