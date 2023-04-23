## 1 STL 概论与版本简介

- 略

## 2 空间配置器

- new 与 delete 关键字
- operator new 与 opetator delete：本质就是对 malloc 和 free 的简单封装
- 定位 new 表达式
- std::destroy 系列：会萃取是否含有 trivial dtor，__has_trivial_destructor(T) 为内置函数，判断类型 T 是否有 trivial destructor。如果有（例如 int 等基础类型），特化到什么都不做的 \__destroy 版本，否则特化到逐个调用析构函数的版本。
- malloc_alloc 实现（一级适配器）：分配失败时，会尝试调用 oom_malloc / oom_realloc 句柄，这两个句柄可能可以帮助腾出内存。句柄需要手动设置，默认为空。
- default_alloc 实现（二级适配器）：（现定义为 pool_allocator）
  - 8，16，...，128 字节数组链表。
  - 链表为空则调用 refill 函数补充节点（正常情况一次性补充 20个，内存不足时则稍少）。
  - refill 函数调用 chunk_alloc 函数获得内存。chunk_alloc 维持一个内存池。池中内存不足时，调用 malloc 获得内存，malloc 失败会收集链表数组中空闲的节点，链表中都找不到了会尝试使用 malloc_alloc 的失败处理句柄，再不行才报异常。
- uninitialized_copy，uninitialized_fill，uninitialized_fill_n。
  - 三者都具有回滚语义。
  - 三者都对 POD 类型有特化，即不调用复制构造函数，直接 调用 = 赋值。
  - uninitialized_copy 对 char* 和 w_char* 有额外特化：直接 memmove。

## 3 迭代器

#### associated types

- value_type

- difference_type

- pointer

- reference

- iterator_category
  - input_iterator_tag：只读迭代器
  - output_iterator_tag：只写迭代器
  - forward_iterator_tag：前向迭代器，继承自 input_iterator_tag
  - bidirectional_iterator_tag：双向迭代器，继承自 forward_iterator_tag
  - random_access_iterator_tag：随机访问迭代器，继承自 bidirectional_iterator_tag
  - contiguous_iterator_tag_tag：连续迭代器，继承自 random_access_iterator_tag

#### iterator_concepts

- input_iterator
- output_iterator
- forward_iterator
- bidirectional_iterator
- random_access_iterator
- contiguous_iterator

#### iterator_traits

- 裸指针和 pointer to const 不具有 associated types。
- 对裸指针和 pointer to const 进行 iterator_traits 特化，保证 iterator_traits 对裸指针和 pointer to const 仍然具有合理意义。

#### std::advance

```cpp
template<typename _InputIterator, typename _Distance>
inline _GLIBCXX17_CONSTEXPR void
advance(_InputIterator& __i, _Distance __n) {
  // concept requirements -- taken care of in __advance
  typename iterator_traits<_InputIterator>::difference_type __d = __n;
  std::__advance(__i, __d, std::__iterator_category(__i));
}

template<typename _InputIterator, typename _Distance>
inline _GLIBCXX14_CONSTEXPR void
__advance(_InputIterator& __i, _Distance __n, input_iterator_tag) {
  // concept requirements
  __glibcxx_function_requires(_InputIteratorConcept<_InputIterator>)
  __glibcxx_assert(__n >= 0);
  while (__n--) ++__i;
}

template<typename _BidirectionalIterator, typename _Distance>
inline _GLIBCXX14_CONSTEXPR void
__advance(_BidirectionalIterator& __i, _Distance __n,
	        bidirectional_iterator_tag) {
  // concept requirements
  __glibcxx_function_requires(_BidirectionalIteratorConcept<
				    _BidirectionalIterator>)
  if (__n > 0)
    while (__n--) ++__i;
  else
    while (__n++) --__i;
}

template<typename _RandomAccessIterator, typename _Distance>
inline _GLIBCXX14_CONSTEXPR void
__advance(_RandomAccessIterator& __i, _Distance __n,
           random_access_iterator_tag) {
  // concept requirements
  __glibcxx_function_requires(_RandomAccessIteratorConcept<
				   _RandomAccessIterator>)
  if (__builtin_constant_p(__n) && __n == 1) ++__i;
  else if (__builtin_constant_p(__n) && __n == -1) --__i;
  else __i += __n;
}
```

- 例如 forward_iterator_tag，没有直接匹配的 __advance，但 forward_iterator_tag 继承自 input_iterator_tag，所以匹配到 input_iterator_tag 的 \__advance。

- std::distance：与 advance 类似，对 input_iterator_tag 和 random_access_iterator_tag 具有不同的特化版本。

## 4 序列式容器

#### vector

- insert，insert_aux。
- 重新分配内存会导致迭代器失效。

#### list

- 节点的实现：list_node_base，list_node。

  - 头节点实现为 list_node_base 的子类，附加一个 count 用于节点计数。
  - 普通节点实现为 list_node_base 的子类，附加一个 数据 buffer，注意 buffer 实现为 char 数组，使用 align_as 按 value_type 对齐。实现为 char 数组的 buffer 是因为新建节点后不一定会立即调用构造函数。

- 链表的实现：带头节点的双向链表。

- transfer 函数：迭代器内置函数，用于链接的重定向。

- splice 系列函数：在指定位置插入另一个链表中的一个节点或一段节点，此处 “另一个链表” 可以是自身。

  ```cpp
  // __x 不能为自身
  void splice(const_iterator __position, list &__x);	
  void splice(const_iterator __position, list &&__x);
  
  // 如果 __x 是自身，__position 不能是 __i 或 __i + 1。
  void splice(const_iterator __position, list &__x, const_iterator __i);
  void splice(const_iterator __position, list &&__x, const_iterator __i);
  
  // 如果 __x 是自身，__position 不能在 [__first, __last] 之间。
  void splice(const_iterator __position, list &__x, 
              const_iterator __first, const_iterator __last);
  void splice(const_iterator __position, list &__x, 
              const_iterator __first, const_iterator __last);
  ```

- remove，remove_if 函数：根据值/条件删除节点。

- unique 函数：根据值/条件删除重复。注意调用前必须保证链表已有序。

- merge 函数：根据小于/条件与另一个链表二路归并。要求本链表和另一个链表都已经有序。归并到本链表，另一个链表变为空。

- reverse 函数：反转链表。

- sort 函数：归并排序，迭代实现，简要算法思路如下。

  ```cpp
  template <class T, class Alloc>
  void list<T, Alloc>::sort() {
    if (size() == 0 || size() == 1) return;
    list carry, counter[64];	// 最多可归并元素数量：2^64 - 1。
    int fill = 0;
    
    while (!empty()) {
      carry.splice(carry.begin(), *this, begin());
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry);
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      if (i == fill) ++fill;
    }
    
    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i-1]);
    swap(counter[fill-1]);
  }
  ```

#### deque

- deque的排序：先复制到 vector 容器排序，再写回 deque。

- deque的中控器

  ```cpp
  template <class T, class Alloc=alloc>
  class deque {
  typedef T value_type;
  typedef value_type* pointer;
  typedef pointer* map_pointer;
    
   protected:
  	map_pointer map;
    size_type map_size;
    iterator start;
    iterator finish;
  }
  ```

- deque的迭代器

  ```cpp
  template <class T, class Ref, class Ptr>
  struct __deque_iterator {
    // ...
    T* cur;
    T* first;
    T* last;
    map_pointer node;
  }
  ```

#### stack

#### queue

#### heap

#### priority_queue

#### forward_list

## 5 关联式容器

#### rb_tree

#### set

#### map

#### multiset

#### multimap

#### hash_set

#### hash_map

#### hash_multiset

#### hash_multimap

## 6 STL算法

#### numeric

- accumulate
- adjacent_difference
- inner_product
- partial_sum
- power
- itoa

#### algobase

- fill
- fill_n
- iter_swap
- iexicographical_compare
- max, min
- mismatch
- swap
- copy
- copy_backward

#### 集合算法

- set_union
- set_intersection
- set_difference
- set_symmetric_difference

#### heap算法

- make_heap
- push_heap
- pop_heap
- sort_heap

#### 其他常见算法

- lower_bound
- upper_bound
- binary_search
- next_permutation
- prev_permutation
- random_shuffle
- partial_sort
- partial_sort_copy
- sort
- equal_range
- inplace_merge
- nth_element
- merge sort

## 7 仿函数

#### unary_function

#### binary_function

## 8 适配器

#### 容器适配器

#### 迭代器适配器

#### 函数适配器

