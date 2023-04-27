## 二叉树中和为某一值的路径

--------------------

- 给你二叉树的根节点 root 和一个整数目标和 targetSum ，找出所有 从根节点到叶子节点 路径总和等于给定目标和的路径。

- 叶子节点 是指没有子节点的节点。

--------------------

- 示例 1：
    ```txt
           5
         /  \
        4    8
       /    /  \
      11   13   4
     /  \      / \
    7    2    5   1
    ```

    - 输入：root = [5,4,8,11,null,13,4,7,2,null,null,5,1], targetSum = 22
    
    - 输出：[[5,4,11,2],[5,8,4,5]]

--------------------

- 限制：

    - 数组长度 <= 1000

--------------------

- 回溯法

```cpp
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    vector<vector<int>> pathSum(TreeNode* root, int target) {
        if (root == nullptr) return {};
        vector<int> seq;
        vector<vector<int>> res;
        func(res, seq, root, target);
        return res;
    }
    void func(vector<vector<int>> &res, vector<int> &seq, TreeNode* root, int sum) {
        seq.push_back(root->val);
        sum -= root->val;
        if (root->left == nullptr && root->right == nullptr) {
            if (sum == 0) res.push_back(seq);
        } else {
            if (root->left) func(res, seq, root->left, sum);
            if (root->right) func(res, seq, root->right, sum);
        }
        seq.pop_back();
    }
};
```