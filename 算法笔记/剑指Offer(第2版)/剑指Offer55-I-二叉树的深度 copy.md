## 二叉树的深度

--------------------

- 输入一棵二叉树的根节点，求该树的深度。从根节点到叶节点依次经过的节点（含根、叶节点）形成树的一条路径，最长路径的长度为树的深度。

--------------------

- 示例 1:

    - 输入: 给定二叉树 [3,9,20,null,null,15,7]，

    ```txt
        3
       / \
      9  20
        /  \
       15   7
    ```

    - 输出: 4

--------------------

- 限制：

    - 节点总数 <= 10000

--------------------

- 基础题

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
    int maxDepth(TreeNode* root) {
        /// 递归方式
        // if (root == nullptr) return 0;
        // return max(maxDepth(root->left), maxDepth(root->right)) + 1;
        
        /// 非递归方式
        stack<TreeNode*> st;
        stack<int> ht;
        int depth = 0, max_depth = 0;
        while (!st.empty() || root != nullptr) {
            while (root != nullptr) {
                st.push(root);
                ht.push(++depth);
                root = root->left;
            }
            root = st.top();
            if (!root->right && ht.top() > max_depth)
                max_depth = ht.top();
            root = root->right;
            depth = ht.top();
            st.pop();
            ht.pop();
        }
        return max_depth;
    }
};
```