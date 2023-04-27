## 删除链表的节点

--------------------

- 给定单向链表的头指针和一个要删除的节点的指针，定义一个函数删除该节点。

--------------------

- 示例 1：
    
    - 输入: head = [4,5,1,9], *pos = 5

    - 输出: [4,1,9]

    - 解释: 给定你链表中值为 5 的第二个节点，那么在调用了你的函数之后，该链表应变为 4 -> 1 -> 9.

--------------------

- 限制：

    - 题目保证链表中节点的值互不相同

    - pos 节点一定存在于链表中
    
    - 若使用 C 或 C++ 语言，你不需要 free 或 delete 被删除的节点

--------------------

- 方法一：顺序遍历.

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
    ListNode* deleteNode(ListNode* head, ListNode* pos) {
        ListNode guard;
        guard.next = head;
        ListNode* pre = &guard, *p = pre->next;
        while (p != pos) {
            pre = p;
            p = p->next;
        }
        pre->next = p->next;
        return guard.next;
    }
};
```
--------------------

- 方法二：修改数据

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
    ListNode* deleteNode(ListNode* head, ListNode* pos) {
        if (pos->next != nullptr) {
            pos->val = pos->next->val;
            pos->next = pos->next->next;
            return head;
        } else {
            ListNode guard;
            guard.next = head;
            ListNode* pre = &guard, *p = pre->next;
            while (p != pos) {
                pre = p;
                p = p->next;
            }
            pre->next = p->next;
            return guard.next;
        }
    }
};
```