## 复杂链表的复制

--------------------

- 请实现 copyRandomList 函数，复制一个复杂链表。在复杂链表中，每个节点除了有一个 next 指针指向下一个节点，还有一个 random 指针指向链表中的任意节点或者 null。

--------------------

- 限制：

    - -10000 <= Node.val <= 10000
    
    - Node.random 为空（null）或指向链表中的节点。

    - 节点数目不超过 1000 。

--------------------

- 迭代 + 节点拆分

```cpp
/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};
*/
class Solution {
public:
    Node* copyRandomList(Node* head) {
        if (head == nullptr) return nullptr;
        Node* cur = head, *next;
        while (cur != nullptr) {
            Node* tp = cur;
            cur = cur->next;
            Node* nd = new Node {tp->val};
            tp->next = nd;
            nd->next = cur;
        }
        cur = head;
        while (cur != nullptr) {
            if (cur->random != nullptr)
                cur->next->random = cur->random->next;
            cur = cur->next->next;
        }
        cur = head;
        Node* res = head->next;
        while (true) {
            Node* tmp = cur->next;
            cur->next = cur->next->next;
            if (cur->next == nullptr)
                break;
            tmp->next = cur->next->next;
            cur = cur->next;
        }
        return res;
    }
};
```