## 反转链表

--------------------

- 定义一个函数，输入一个链表的头节点，反转该链表并输出反转后链表的头节点。

--------------------

- 示例 1：

    - 输入: 1->2->3->4->5->NULL
    
    - 输出: 5->4->3->2->1->NULL

--------------------

- 限制：

    - 0 <= 节点个数 <= 5000

--------------------

- 简单题

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
    ListNode* reverseList(ListNode* head) {
        ListNode* pre = nullptr;
        while (head != nullptr) {
            ListNode* next = head->next;
            head->next = pre;
            pre = head;
            head = next;
        }
        return pre;
    }
};
```