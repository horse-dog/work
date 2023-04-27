## 找出数组中重复的数字

--------------------

- 在一个长度为 n 的数组 nums 里的所有数字都在 0～n-1 的范围内。数组中某些数字是重复的，但不知道有几个数字重复了，也不知道每个数字重复了几次。请找出数组中任意一个重复的数字。

--------------------

- 示例 1：

    - 输入：
    [2, 3, 1, 0, 2, 5, 3]

    - 输出：2 或 3 

--------------------

- 限制：

    - 2 <= n <= 100000

--------------------

```cpp
class Solution {
public:
    int findRepeatNumber(vector<int> &nums) {
        unordered_set<int> dict;
        for (auto &&num : nums) {
            if (dict.find(num) == dict.end())
                dict.insert(num);
            else
                return num;
        }
        return -1;
    }
};
```
--------------------

- 方法一: 直接使用哈希表即可

    - 时间复杂度: O(n)

    - 空间复杂度: O(n)

- 方法二: 与哈希表原理相同,但使用位图

    - 由于限制条件里 n 最大为 100000, 所以需要分配的内存并不大

    - 时间复杂度: O(n)

    - 空间复杂度: O(n)
    
    ```cpp
    class Solution {
    private:
        struct bitmap {
            int data[100000 / 32] = {0};

            bool test(size_t i) {
                size_t r = i / 32;
                size_t c = i % 32;
                int tmp = 1 << c;
                return data[r] & tmp;
            }

            void set(size_t i) {
                size_t r = i / 32;
                size_t c = i % 32;
                int tmp = 1 << c;
                data[r] = data[r] | tmp;
            }
        };
    
    public:
        int findRepeatNumber(vector<int> &nums) {
            Solution::bitmap dict;
            for (auto &&num : nums) {
                if (dict.test(num) == false)
                    dict.set(num);
                else
                    return num;
            }
            return -1;
        }
    };
    ```