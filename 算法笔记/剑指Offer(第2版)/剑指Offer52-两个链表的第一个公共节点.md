## 两个链表的第一个公共节点

--------------------

- 输入两个链表，找出它们的第一个公共节点。

--------------------

- 限制：

    - 如果两个链表没有交点，返回 null.
    
    - 在返回结果后，两个链表仍须保持原有的结构。

    - 可假定整个链表结构中没有循环。

    - 程序尽量满足 O(n) 时间复杂度，且仅用 O(1) 内存。

--------------------

- 链表经典题

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
    ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
        if (headA == nullptr || headB == nullptr)
            return nullptr;
        auto nodeA = headA, nodeB = headB;
        bool switchedA = false, switchedB = false;
        while (nodeA != nodeB) {
            if (nodeA->next != nullptr) {
                nodeA = nodeA->next;
            } else {
                if (switchedA) return nullptr;
                switchedA = true;
                nodeA = headB;
            }
            if (nodeB->next != nullptr) {
                nodeB = nodeB->next;
            } else {
                if (switchedB) return nullptr;
                switchedB = true;
                nodeB = headA;
            }
        }
        return nodeA;
    }
};
```