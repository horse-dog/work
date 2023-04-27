## 从尾到头打印链表

--------------------

- 输入一个链表的头节点，从尾到头反过来返回每个节点的值（用数组返回）。

--------------------

- 示例 1：

    - 输入：head = [1,3,2]
    - 输出：[2,3,1]

--------------------

- 限制：

    - 0 <= 链表长度 <= 10000

--------------------

```cpp
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    vector<int> reversePrint(ListNode* head) {
        size_t n;
        ListNode* p = head;
        for (; p != nullptr; p = p->next);
        vector<int> res(n);
        p = head;
        for (auto it = res.rbegin(); it != res.rend();
             ++it; p = p->next)
            *it = p->val;
        return res;
    }
};
```
--------------------

- 步骤一: 遍历链表得到链表长度 n

- 步骤二: vector<int> res{n};

- 步骤三: 从尾到头对 vector 赋值