## 旋转数组的最小数字

--------------------

- 把一个数组最开始的若干个元素搬到数组的末尾，我们称之为数组的旋转。

    - 给你一个可能存在**重复**元素值的数组 `numbers` ，它原来是一个升序排列的数组，并按上述情形进行了一次旋转。请返回旋转数组的**最小元素**。例如，数组 `[3,4,5,1,2]` 为 `[1,2,3,4,5]` 的一次旋转，该数组的最小值为 1。  

    - 注意，数组 `[a[0], a[1], a[2], ..., a[n-1]]` 旋转一次 的结果为数组 `[a[n-1], a[0], a[1], a[2], ..., a[n-2]]` 。


--------------------

- 示例 1：

    - 输入：numbers = [3,4,5,1,2]

    - 输出：1

- 示例 2:

    - 输入：numbers = [2,2,2,0,1]

    - 输出：0

--------------------

- 限制：

    - n == numbers.length

    - 1 <= n <= 5000
    
    - -5000 <= numbers[i] <= 5000

    - numbers 原来是一个升序排序的数组，并进行了 1 至 n 次旋转

--------------------

```cpp
class Solution {
public:
    int minArray(vector<int>& numbers) {
        for (int i = 1; i < numbers.size(); i++) {
            if (numbers[i - 1] > numbers[i])
                return numbers[i];
        }
        return numbers[0];
    }
};
```
--------------------

- 方法一：顺序遍历：O(n)

- 方法二：二分查找：O(logn)

    - 我们考虑数组中的最后一个元素 x：
        
        - 在最小值右侧的元素，它们的值一定都小于等于 x
        
        - 在最小值左侧的元素，它们的值一定都大于等于 x

    - 在二分查找的每一步中，左边界为 low，右边界为 high，区间的中点为 pivot，最小值就在该区间内。

        - 若 `numbers[pivot]` < `numbers[high]`: 这说明 `numbers[pivot]` 是最小值右侧的元素，因此我们可以忽略二分查找区间的右半部分。

        - 若 `numbers[pivot]` > `numbers[high]`: 这说明 `numbers[pivot]` 是最小值左侧的元素，因此我们可以忽略二分查找区间的左半部分。

        - 若 `numbers[pivot]` == `numbers[high]`: 由于重复元素的存在，我们并不能确定 `numbers[pivot]` 究竟在最小值的左侧还是右侧，但可以忽略二分查找区间的右端点。

--------------------

```cpp
class Solution {
public:
    int minArray(vector<int>& numbers) {
        int low = 0;
        int high = numbers.size() - 1;
        while (low < high) {
            int pivot = low + (high - low) / 2;
            if (numbers[pivot] < numbers[high]) {
                high = pivot;
            }
            else if (numbers[pivot] > numbers[high]) {
                low = pivot + 1;
            }
            else {
                high -= 1;
            }
        }
        return numbers[low];
    }
};
```