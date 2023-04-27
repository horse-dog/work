## 用两个栈实现队列

--------------------

- 用两个栈实现一个队列。队列的声明如下，请实现它的两个函数 appendTail 和 deleteHead ，分别完成在队列尾部插入整数和在队列头部删除整数的功能。(若队列中没有元素，deleteHead 操作返回 -1 )。

--------------------

- 示例 1：

    - 输入：

    ```text
    ["CQueue","appendTail","deleteHead","deleteHead","deleteHead"]
    [[],[3],[],[],[]]
    ```

    - 输出：[null,null,3,-1,-1]

- 示例 2:

    - 输入：

    ```text
    ["CQueue","deleteHead","appendTail","appendTail","deleteHead","deleteHead"]
    [[],[],[5],[2],[],[]]
    ```

    - 输出：[null,-1,null,null,5,2]

--------------------

- 限制：

    - 1 <= values <= 10000

    - 最多会对 appendTail、deleteHead 进行 10000 次调用

--------------------

```cpp
class CQueue {
private:
    stack<int> st1, st2;

public:
    CQueue() {

    }
    
    void appendTail(int value) {
        st2.push(value);
    }
    
    int deleteHead() {
        if (st1.empty()) {
            while (!st2.empty()) {
                st1.push(st2.top());
                st2.pop();
            }
        }
        if (st1.empty())
            return -1;
        else {
            int res = st1.top();
            st1.pop();
            return res;
        }
    }
};

/**
 * Your CQueue object will be instantiated and called as such:
 * CQueue* obj = new CQueue();
 * obj->appendTail(value);
 * int param_2 = obj->deleteHead();
 */
```
--------------------

- 数据结构基础题
