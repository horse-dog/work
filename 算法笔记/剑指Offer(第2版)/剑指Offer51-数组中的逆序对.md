## 数组中的逆序对

--------------------

- 在数组中的两个数字，如果前面一个数字大于后面的数字，则这两个数字组成一个逆序对。输入一个数组，求出这个数组中的逆序对的总数。

--------------------

- 示例 1：

    - 输入: [7,5,6,4]
    
    - 输出: 5

--------------------

- 限制：

    - 0 <= 数组长度 <= 50000

--------------------

- 归并排序，递归时逐层统计逆序对数

```cpp
class Solution {
public:
    int reversePairs(vector<int>& nums) {
        vector<int> aux(nums.size());
        return merge_sort(nums, 0, nums.size(), aux);
    }

    int merge_sort(vector<int>& nums, int l, int r, 
                   vector<int>& aux) {
        if (l == r || l + 1 == r) return 0;
        int m  = (l + r) >> 1;
        int res = merge_sort(nums, l, m, aux)
                + merge_sort(nums, m, r, aux);
        
        for (int i = l; i < r; i++)
            aux[i] = nums[i];
        
        int il = l, ir = m;
        for (int i = l; i < r; i++) {
            if (il == m) 
                nums[i] = aux[ir++];
            else if (ir == r || aux[il] <= aux[ir]) 
                nums[i] = aux[il++];
            else {
                res += m - il;
                nums[i] = aux[ir++];
            }
        }
        
        return res; 
    }
};
```