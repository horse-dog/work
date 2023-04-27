## 调整数组顺序使奇数位于偶数前面

--------------------

- 输入一个整数数组，实现一个函数来调整该数组中数字的顺序，使得所有奇数在数组的前半部分，所有偶数在数组的后半部分。

--------------------

- 示例 1：

    - 输入：nums = [1,2,3,4]

    - 输出：[1,3,2,4] 

    - 注：[3,1,2,4] 也是正确的答案之一。

--------------------

- 限制：

    - 0 <= nums.length <= 50000
    
    - 0 <= nums[i] <= 10000

--------------------

- 快排 partition 函数的思路：原地交换

```cpp
class Solution {
public:
    vector<int> exchange(vector<int>& nums) {
        if (nums.size() == 0) return {};
        int l = 0, r = nums.size() - 1;
        int* data = nums.data();
        for (; (l < r) && (data[l] & 1); ) ++l;
        int tmp = data[l];
        while (l < r) {
            while (l < r && !(data[r] & 1)) --r;
            data[l] = data[r];
            while (l < r && (data[l] & 1)) ++l;
            data[r] = data[l];
        }
        data[l] = tmp;
        return nums;
    }
};
```
