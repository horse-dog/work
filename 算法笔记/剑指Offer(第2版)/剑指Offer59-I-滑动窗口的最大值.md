## 滑动窗口的最大值

--------------------

- 给定一个数组 nums 和滑动窗口的大小 k，请找出所有滑动窗口里的最大值。

--------------------

- 示例:

    - 输入: nums = [1,3,-1,-3,5,3,6,7], 和 k = 3

    - 输出: [3,3,5,5,6,7] 

    - 解释: 

        ```txt
          滑动窗口的位置                最大值
        ---------------               -----
        [1  3  -1] -3  5  3  6  7       3
        1  [3  -1  -3] 5  3  6  7       3
        1   3 [-1  -3  5] 3  6  7       5
        1   3  -1 [-3  5  3] 6  7       5
        1   3  -1  -3 [5  3  6] 7       6
        1   3  -1  -3  5 [3  6  7]      7
        ```

--------------------

- 限制：

    - 你可以假设 k 总是有效的，在输入数组 不为空 的情况下，1 ≤ k ≤ nums.length。

--------------------

- 使用单调队列维护最大值

```cpp
class Solution {
    struct aux_queue {
        deque<int> dq {};
        void pop(int x) {
            if (dq.front() == x) dq.pop_front(); 
        }
        void push(int x) {
            while (!dq.empty() && dq.back() < x) dq.pop_back();
            dq.push_back(x);
        }
        int top() { return dq.front(); }
    };
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        vector<int> res;
        aux_queue aq;
        res.reserve(nums.size() - k + 1);
        for (int i = 0; i < k; i++) aq.push(nums[i]);
        res.push_back(aq.top());
        for (int i = 0, j = k; j < nums.size(); i++, j++) {
            aq.pop(nums[i]);
            aq.push(nums[j]);
            res.push_back(aq.top());
        }
        return res;
    }
};
```