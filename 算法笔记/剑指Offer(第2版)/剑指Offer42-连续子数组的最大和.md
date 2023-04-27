## 连续子数组的最大和

--------------------

- 输入一个整型数组，数组中的一个或连续多个整数组成一个子数组。求所有子数组的和的最大值。

- 要求时间复杂度为O(n)。

--------------------

- 示例 1：

    - 输入: nums = [-2,1,-3,4,-1,2,1,-5,4]

    - 输出: 6

    - 解释: 连续子数组 [4,-1,2,1] 的和最大，为 6。

--------------------

- 限制：

    - 1 <= arr.length <= 10^5
    
    - -100 <= arr[i] <= 100

--------------------

- 动态规划：

    - 记所有以 ai 为右边界的子数组的和的最大值为 mi，则:
    
    ```text
    if mi-1 > 0:
        mi = mi-1 + ai
    else
        mi = ai
    ```

```cpp
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int res = nums[0], tmp = res;
        for (int i = 1; i < nums.size(); i++) {
            if (tmp < 0) 
                tmp = nums[i];
            else 
                tmp += nums[i];
            if (res < tmp) res = tmp;
        }
        return res;
    }
};
```