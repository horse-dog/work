## 在排序数组中查找数字I

--------------------

- 统计一个数字在排序数组中出现的次数。

--------------------

- 示例 1:

    - 输入: nums = [5,7,7,8,8,10], target = 8

    - 输出: 2

- 示例 2:

    - 输入: nums = [5,7,7,8,8,10], target = 6

    - 输出: 0

--------------------

- 限制：

    - 0 <= nums.length <= 10^5
    
    - -10^9 <= nums[i] <= 10^9

    - nums 是一个非递减数组

    - -10^9 <= target <= 10^9

--------------------

- 二分查找，实现 lower_bound 和 upper_bound

```cpp
class Solution {
public:
    int search(vector<int>& nums, int target) {
        return upper_bnd(nums, target) - lower_bnd(nums, target);
    }

    // lower_bound = min{ i | vc[i] >= target }
    int lower_bnd(vector<int>& vc, int target) {
        int l = 0, r = vc.size() - 1;
        while (l <= r) {
            int m = (l + r) >> 1;
            if (vc[m] >= target) 
                r = m - 1;
            else 
                l = m + 1;
        }
        return l;
    }

    // lower_bound = min{ i | vc[i] > target }
    int upper_bnd(vector<int>& vc, int target) {
        int l = 0, r = vc.size() - 1;
        while (l <= r) {
            int m = (l + r) >> 1;
            if (vc[m] > target) 
                r = m - 1;
            else 
                l = m + 1;
        }
        return l;
    }
};
```