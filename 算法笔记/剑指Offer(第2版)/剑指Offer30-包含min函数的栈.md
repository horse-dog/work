## 包含min函数的栈

--------------------

- 定义栈的数据结构，请在该类型中实现一个能够得到栈的最小元素的 min 函数在该栈中，调用 min、push 及 pop 的时间复杂度都是 O(1)。

--------------------

- 示例:

    - MinStack minStack = new MinStack();

    - minStack.push(-2);

    - minStack.push(0);

    - minStack.push(-3);

    - minStack.min();   --> 返回 -3.

    - minStack.pop();

    - minStack.top();      --> 返回 0.

    - minStack.min();   --> 返回 -2.

--------------------

- 限制：

    - 各函数的调用总次数不超过 20000 次

--------------------

- 额外添加一个栈记录当前的最小值

```cpp
class MinStack {
    stack<int> x_stack;
    stack<int> min_stack;
public:
    MinStack() {
        min_stack.push(INT_MAX);
    }
    
    void push(int x) {
        x_stack.push(x);
        min_stack.push(::min(min_stack.top(), x));
    }
    
    void pop() {
        x_stack.pop();
        min_stack.pop();
    }
    
    int top() {
        return x_stack.top();
    }
    
    int min() {
        return min_stack.top();
    }
};
```