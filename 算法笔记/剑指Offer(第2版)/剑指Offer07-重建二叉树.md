## 重建二叉树

--------------------

- 输入某二叉树的前序遍历和中序遍历的结果，请构建该二叉树并返回其根节点。假设输入的前序遍历和中序遍历的结果中都不含重复的数字。

--------------------

- 示例 1：

    - 输入：preorder = [3,9,20,15,7], inorder = [9,3,15,20,7]

    - 输出：[3,9,20,null,null,15,7]

- 示例 2:

    - 输入：preorder = [-1], inorder = [-1]

    - 输出：[-1]

--------------------

- 限制：

    - 0 <= 节点个数 <= 5000

--------------------

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
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        assert(preorder.size() == inorder.size());
        TreeNode* root = __build(preorder, inorder,
                                 0, 0, preorder.size());
        return root;
    }
    TreeNode* __build(const vector<int> &preorder, 
                      const vector<int> &inorder,
                      int lbegin, int rbegin, int len) {
        
        if (len == 0) return nullptr;
        TreeNode* root = new TreeNode(preorder[lbegin]);
        int n = 0;
        for (int i = rbegin; inorder[i] != root->val; i++, n++);
        root->left  = __build(preorder, inorder, lbegin + 1, rbegin, n);
        root->right = __build(preorder, inorder, lbegin + 1 + n, rbegin + n + 1, len - n - 1);
        return root;
    }
};
```
--------------------

- 数据结构基础题
