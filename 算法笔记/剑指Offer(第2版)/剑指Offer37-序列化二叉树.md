## 序列化二叉树

--------------------

- 请实现两个函数，分别用来序列化和反序列化二叉树。

- 你需要设计一个算法来实现二叉树的序列化与反序列化。这里不限定你的序列 / 反序列化算法执行逻辑，你只需要保证一个二叉树可以被序列化为一个字符串并且将这个字符串反序列化为原始的树结构。

- 提示：输入输出格式与 LeetCode 目前使用的方式一致，详情请参阅 LeetCode 序列化二叉树的格式。你并非必须采取这种方式，你也可以采用其他的方法解决这个问题。

--------------------

- 括号表示法

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
class Codec {
public:

    // Encodes a tree to a single string.
    string serialize(TreeNode* root) {
        string res {};
        if (root != nullptr)
            do_serialize(root, res);
        return res;
    }

    // Decodes your encoded data to tree.
    TreeNode* deserialize(string data) {
        TreeNode *root;
        char* str = (char*)data.c_str();
        root = parse(str);
        return root;
    }

    void do_serialize(TreeNode* nd, string& str) {
        str += to_string(nd->val);
        if (nd->left || nd->right) {
            str += '(';
            if (nd->left) do_serialize(nd->left, str);
            if (nd->right) {
                str += ',';
                do_serialize(nd->right, str);
            }
            str += ')';
        }
    }

    TreeNode* parse(char*& str) {
        if (*str == '\0' || *str == ',') return nullptr;
        TreeNode* node = new TreeNode {0};
        node->val = (int)strtol(str, &str, 10);
        if (*str == '(') {
            node->left = parse(++str);
            if (*str == ',')
                node->right = parse(++str);
            ++str; // this is ')'.
        }
        return node;
    }
};

// Your Codec object will be instantiated and called as such:
// Codec codec;
// codec.deserialize(codec.serialize(root));
```