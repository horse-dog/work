## 数组中数字出现的次数

--------------------

- 一个整型数组 nums 里除两个数字之外，其他数字都出现了两次。请写程序找出这两个只出现一次的数字。要求时间复杂度是O(n)，空间复杂度是O(1)。

--------------------

- 示例 1：

    - 输入：nums = [4,1,4,6]
    
    - 输出：[1,6] 或 [6,1]

- 示例 2：

    - 输入：nums = [1,2,10,4,1,4,3,3]

    - 输出：[2,10] 或 [10,2]

--------------------

- 限制：

    - 2 <= nums.length <= 10000

--------------------

- 记需要查找的两个数为 a 和 b。

- 对所有元素异或，则结果为 a ^ b。

- 找到 a ^ b 里面一个为 1 的位，易知在这个位上 a 和 b 一定是不同的。

- 再次遍历和异或，以在上一步得到的位是否为1为划分依据。

```cpp
class Solution {
public:
    vector<int> singleNumbers(vector<int>& nums) {
        int axorb = 0, div = 1, a = 0, b = 0;
        for (auto num : nums) axorb ^= num;
        while (!(div & axorb)) div <<= 1;
        for (auto num : nums)
            (div & num) ? a ^= num : b ^= num;
        return {a, b};
    }
};
```