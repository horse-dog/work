## 合并两个排序的链表

--------------------

- 输入两个递增排序的链表，合并这两个链表并使新链表中的节点仍然是递增排序的。

--------------------

- 示例 1：

    - 输入：1->2->4, 1->3->4

    - 输出：1->1->2->3->4->4

--------------------

- 限制：

    - 0 <= 链表长度 <= 1000

--------------------

- 简单题，归并排序

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
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode guard{0};
        ListNode *cur = &guard;
        while (l1 != nullptr && l2 != nullptr) {
            if (l1->val < l2->val) {
                cur->next = l1;
                l1 = l1->next;
            } else {
                cur->next = l2;
                l2 = l2->next;
            }
            cur = cur->next;
        }
        if (l1 != nullptr) 
            cur->next = l1;
        else 
            cur->next = l2;
        return guard.next;
    }
};
```