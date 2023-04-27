#pragma once

#include <atomic>
#include <memory>
#include <sstream>
#include <iostream>

#define __TINY_MEMPOOL__

class tiny_mempool {
 protected:
 struct memNode { memNode *nextnode = nullptr; };

 protected:
  std::atomic<memNode*> m_free_head[16];

 private:
  tiny_mempool() {}

  ~tiny_mempool()
  { for (int i = 0; i < 16; i++)
    { if (m_free_head[i] != nullptr)
      { memNode *ptr = m_free_head[i];
        while (ptr != nullptr)
        { auto nptr = ptr->nextnode;
          free(ptr);
          ptr = nptr;
        }
      }
      m_free_head[i] = nullptr;
    }
  }

  int getindex(int size)
  { static const unsigned int sizetable[16]
    = { 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128 };
    int __distance = 0;
    for (; __distance < 16; __distance++)
      if (sizetable[__distance] >= size)
        break;
    return __distance;
  }

 public:
  static tiny_mempool &instance()
  { static tiny_mempool pool;
    return pool;
  }

 public:
  void *alloc(int size)
  { if (size > 128) return malloc(size);
    int index = getindex(size);
    int realsize = (index + 1) << 3;
    memNode *p = m_free_head[index];
    if (p == nullptr)
      return malloc(realsize);
    else
    { while (!m_free_head[index].compare_exchange_weak(p, p->nextnode))
        if (p == nullptr) return malloc(realsize);
      return p;
    }
    return nullptr;
  }

  void delloc(void *ptr, int size)
  { if (ptr == nullptr) return;
    if (size > 128) return free(ptr);
    int index = getindex(size);
    memNode *pNew = (memNode *)ptr;
    pNew->nextnode = m_free_head[index];
    while (!(m_free_head[index].compare_exchange_weak(pNew->nextnode, pNew)))
    ;
  }
  
  /**
   * @brief report memory distribute in the pool.
   * @attention May cause undefined result if 
   * allocate memory use current pool before this
   * function return.
   */
  void report()
  { printf("\033[32m\033[1mtiny_mempool report\033[0m\n");
    printf("\033[34mindex\tnode size   node count\033[0m\n");
    for (int i = 0; i < 16; ++i)
    { int n = 0;
      memNode *p = m_free_head[i];
      while (p)
      { n++;
        p = p->nextnode;
      }
      printf("\033[31m%5d\t %3d \033[35mbyte\033[31m   %10d"
             "\033[0m\n", i, (i + 1) << 3, n);
    }
  }

};

template<class T>
class tiny_allocator { 
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;

  tiny_allocator() {}

  tiny_allocator(tiny_allocator const &) {}

  tiny_allocator &operator=(tiny_allocator const &)
  { return *this; }

  template<class Other>
  tiny_allocator(tiny_allocator<Other> const &) {}

  template<class Other>
  tiny_allocator &operator=(tiny_allocator<Other> const &)
  { return *this; }

  pointer allocate(size_type count)
  { return (pointer)tiny_mempool::instance()
      .alloc(count * sizeof(value_type));
  }

  void deallocate(pointer ptr, size_type count)
  { return tiny_mempool::instance()
      .delloc(ptr, count * sizeof(value_type));
  }
};

template <typename _Tp1, typename _Tp2>
std::ostream& operator<<(std::ostream& os, 
  const std::pair<_Tp1, _Tp2>& pair) {
  return os << '{' << pair.first << ", " << pair.second << '}';
}

template <typename value_type>
struct avlnode {
  value_type m_data;  /* data buffer */
  char m_balance_factor; /* leftHeight - rightHeight */
  avlnode* m_lchild;  /* left child */
  avlnode* m_rchild;  /* right child */
};

template <typename T, typename Alloc=tiny_allocator<T>>
class avltree {
 protected:
  template <typename _Tp, typename _Up>
  struct alloc_rebind {};

  template <template <typename, typename...> class _Template,
            typename _Up, typename _Tp, typename... _Types>
  struct alloc_rebind<_Template<_Tp, _Types...>, _Up>
  { using type = _Template<_Up, _Types...>; };

  using node = avlnode<T>;
  using allocator_type = typename alloc_rebind<Alloc, node>::type;

  struct rbtree_impl : public allocator_type
  { node*  m_root;
    size_t m_node_count;
  };

  rbtree_impl m_impl;

 public:
  avltree() : m_impl() {}

  avltree(const avltree& tree) {
    m_impl.m_root = copyfrom(tree.m_impl.m_root);
    m_impl.m_node_count = tree.m_impl.m_node_count;
  }

  avltree(avltree&& tree) {
    m_impl.m_root = tree.m_impl.m_root;
    m_impl.m_node_count = tree.m_impl.m_node_count;
    tree.reset();
  }

  ~avltree() { clear(); }

 public:
  bool insert(const T& value) {
    int taller = 0;
    bool res = insert_avl(m_impl.m_root, value, taller);
    if (res == true) ++m_impl.m_node_count;
    return res;
  }

  bool erase(const T& value) {
    int shorter = 0;
    bool res = erase_avl(m_impl.m_root, value, shorter);
    if (res == true) --m_impl.m_node_count;
    return res;
  }

  size_t size() const 
  { return m_impl.m_node_count; }

  bool empty() const 
  { return size() == 0; }

  void clear() {
    if (!empty()) {
      __destroy(m_impl.m_root);
      reset();
    }
  }

 // functions for test.
 public:
  bool isavl() const {
    int h;
    return __isavl(m_impl.m_root, h);
  }

  bool __isavl(node* rt, int& h) const {
    if (rt == nullptr) return true;
    int lh = 0, rh = 0;
    if (!(__isavl(rt->m_lchild, lh) 
       && __isavl(rt->m_rchild, rh)))
      return false;
    h = std::max(lh, rh);
    return abs(lh - rh) <= 1;
  }

  void disp() const {
    if (!empty())
      __disp(m_impl.m_root);
    std::cout << std::endl;
  }

  void inorder() const {
    node* p = m_impl.m_root;
    std::stack<node*> st;
    while (!st.empty() || p != nullptr) {
      while (p != nullptr) {
        st.push(p);
        p = p->m_lchild;
      }
      std::cout << st.top()->m_data << ' ';
      p = st.top()->m_rchild;
      st.pop();
    }
    std::cout << '\n';
  }

  void __disp(node* rt) const {
    std::cout << rt->m_data;
    if (rt->m_lchild || rt->m_rchild) {
      std::cout << '(';
      if (rt->m_lchild)
        __disp(rt->m_lchild);
      if (rt->m_rchild) {
        std::cout << ", ";
        __disp(rt->m_rchild);
      }
      std::cout << ')';
    }
  }
 // *******************

 protected:
  void reset() {
    m_impl.m_node_count = 0;
    m_impl.m_root = nullptr;
  }

  node* copyfrom(const node* rt) {
    if (rt == nullptr) return nullptr;
    node* p = create_node(rt->m_data);
    p->m_balance_factor = rt->m_balance_factor;
    p->m_lchild = copyfrom((const node*)rt->m_lchild);
    p->m_rchild = copyfrom((const node*)rt->m_rchild);
    return p;
  }

  bool insert_avl(node*& pos, const T&value, int& taller) {
    if (pos == nullptr) {
      pos = create_node(value);
      taller = 1;
      return true;
    }
    if (less(value, pos->m_data)) {
      if (insert_avl(pos->m_lchild, value, taller) == false)
        return false;
      if (taller == 1)
        __insertLprocess(pos, taller);
    } else if (less(pos->m_data, value)) {
      if (insert_avl(pos->m_rchild, value, taller) == false)
        return false;
      if (taller == 1)
        __insertRprocess(pos, taller);
    } else {
      taller = 0;
      return false;
    }
    return true;
  }

  bool erase_avl(node*& pos, const T&value, int& shorter) {
    if (pos == nullptr) return false;
    bool result;
    if (less(value, pos->m_data)) {
      result = erase_avl(pos->m_lchild, value, shorter);
      if (shorter == 1) __eraseLprocess(pos, shorter);
      return result;
    } else if (less(pos->m_data, value)) {
      result = erase_avl(pos->m_rchild, value, shorter);
      if (shorter == 1) __eraseRprocess(pos, shorter);
      return result;
    } else {
      node* tmp = pos;
      if (pos->m_lchild == nullptr) {
        pos = pos->m_rchild;
        destroy_node(tmp);
        shorter = 1;
      } else if (pos->m_rchild == nullptr) {
        pos = pos->m_lchild;
        destroy_node(tmp);
        shorter = 1;
      } else {
        __eraseAssist(tmp, tmp->m_rchild, shorter);
        if (shorter == 1)
          __eraseRprocess(tmp, shorter);
        pos = tmp;
      }
      return true;
    }
  }

  node* create_node(const T& x) { 
    node* tmp = m_impl.allocate(1);
    try {
       new (&tmp->m_data) T(x);
       tmp->m_balance_factor = 0;
       tmp->m_lchild = nullptr;
       tmp->m_rchild = nullptr;
    } catch (...) {
      m_impl.deallocate(tmp, 1);
    }
    return tmp;
  }

  void destroy_node(node* p) {
    (&p->m_data)->~T();
    m_impl.deallocate(p, 1);
  }

  template <typename _Tp>
  static bool less(const _Tp& x, const _Tp& y) 
  { return x < y; }

  template <typename _Tp1, typename _Tp2>
  static bool less(const std::pair<_Tp1, _Tp2>& x, 
            const std::pair<_Tp1, _Tp2>& y) {
    return x.first < y.first;
  }

  template <typename _Tp1, typename _Tp2>
  static bool less(const std::pair<_Tp1, _Tp2>& x, const _Tp1& y) {
    return x.first < y;
  }

  template <typename _Tp1, typename _Tp2>
  static bool less(const _Tp1& x, const std::pair<_Tp1, _Tp2>& y) {
    return x < y.first;
  }

  void __destroy(node* pos) {
    while (pos != nullptr) {
      __destroy(pos->m_rchild);
      node* tmp = pos->m_lchild;
      destroy_node(pos);
      pos = tmp;
    }
  }

  void __eraseAssist(node* pos, node*& left, int& shorter) {
    if (left->m_lchild == nullptr) {
      pos->m_data = std::move(left->m_data);
      pos = left;
      left = left->m_rchild;
      destroy_node(pos);
      shorter = 1;
    } else {
      __eraseAssist(pos, left->m_lchild, shorter);
      if (shorter == 1)
        __eraseLprocess(left, shorter);
    }
  }

  void __insertLprocess(node*& pos, int& taller) {
    node* p1, *p2;
    if (pos->m_balance_factor == 0) {
      pos->m_balance_factor = 1;
      taller = 1;
    } else if (pos->m_balance_factor == -1) {
      pos->m_balance_factor = 0;
      taller = 0;
    } else {
      p1 = pos->m_lchild;

      // p1->m_balance_factor must not be 0.
      if (p1->m_balance_factor == 0) {
        printf("%d: p1->m_balance is 0.\n", __LINE__);
        throw;
      }
      
      if (p1->m_balance_factor == 1) { // LL adjust
        pos->m_lchild = p1->m_rchild;
        p1->m_rchild = pos;
        pos->m_balance_factor = 0;
        p1->m_balance_factor = 0;
        pos = p1;
      } else {  // LR adjust
        p2 = p1->m_rchild;
        p1->m_rchild = p2->m_lchild;
        p2->m_lchild = p1;
        pos->m_lchild = p2->m_rchild;
        p2->m_rchild = pos;

        if (p2->m_balance_factor == 0) {
          pos->m_balance_factor = 0;
          p1->m_balance_factor = 0;
        } else if (p2->m_balance_factor == 1) {
          p1->m_balance_factor = 0;
          pos->m_balance_factor = -1;
        } else {
          p1->m_balance_factor = 1;
          pos->m_balance_factor = 0;
        }

        pos = p2;
        pos->m_balance_factor = 0;
      }
      taller = 0;
    }
  }

  void __insertRprocess(node*& pos, int& taller) {
    node* p1, *p2;
    if (pos->m_balance_factor == 0) {
      pos->m_balance_factor = -1;
      taller = 1;
    } else if (pos->m_balance_factor == 1) {
      pos->m_balance_factor = 0;
      taller = 0;
    } else {
      p1 = pos->m_rchild;

      // p1->m_balance_factor must not be 0.
      if (p1->m_balance_factor == 0) {
        printf("%d: p1->m_balance is 0.\n", __LINE__);
        throw;
      }
      
      if (p1->m_balance_factor == -1) { // RR adjust
        pos->m_rchild = p1->m_lchild;
        p1->m_lchild = pos;
        pos->m_balance_factor = 0;
        p1->m_balance_factor = 0;
        pos = p1;
      } else {  // RL adjust
        p2 = p1->m_lchild;
        p1->m_lchild = p2->m_rchild;
        p2->m_rchild = p1;
        pos->m_rchild = p2->m_lchild;
        p2->m_lchild = pos;

        if (p2->m_balance_factor == 0) {
          pos->m_balance_factor = 0;
          p1->m_balance_factor = 0;
        } else if (p2->m_balance_factor == -1) {
          p1->m_balance_factor = 0;
          pos->m_balance_factor = 1;
        } else {
          p1->m_balance_factor = -1;
          pos->m_balance_factor = 0;
        }

        pos = p2;
        pos->m_balance_factor = 0;
      }
      taller = 0;
    }
  }

  void __eraseLprocess(node*&pos, int& shorter) {
    node* p1, *p2;
    if (pos->m_balance_factor == 1) {
      pos->m_balance_factor = 0;
      shorter = 1;
    } else if (pos->m_balance_factor == 0) {
      pos->m_balance_factor = -1;
      shorter = 0;
    } else {
      p1 = pos->m_rchild;
      if (p1->m_balance_factor == 0) {  // RR
        pos->m_rchild = p1->m_lchild;
        p1->m_lchild = pos;
        p1->m_balance_factor = 1;
        pos->m_balance_factor = -1;
        pos = p1;
        shorter = 0;
      } else if (p1->m_balance_factor == -1) {  // LR
        pos->m_rchild = p1->m_lchild;
        p1->m_lchild = pos;
        pos->m_balance_factor = 0;
        p1->m_balance_factor = 0;
        pos = p1;
        shorter = 1;
      } else {  // RL
        p2 = p1->m_lchild;
        p1->m_lchild = p2->m_rchild;
        p2->m_rchild = p1;
        pos->m_rchild = p2->m_lchild;
        p2->m_lchild = pos;
        if (p2->m_balance_factor == 0) {
          pos->m_balance_factor = 0;
          p1->m_balance_factor = 0;
        } else if (p2->m_balance_factor == -1) {
          pos->m_balance_factor = 1;
          p1->m_balance_factor = 0;
        } else {
          pos->m_balance_factor = 0;
          p1->m_balance_factor = -1;
        }
        p2->m_balance_factor = 0;
        pos = p2;
        shorter = 1;
      }
    }
  }

  void __eraseRprocess(node*&pos, int& shorter) {
    node* p1, *p2;
    if (pos->m_balance_factor == -1) {
      pos->m_balance_factor = 0;
      shorter = 1;
    } else if (pos->m_balance_factor == 0) {
      pos->m_balance_factor = 1;
      shorter = 0;
    } else {
      p1 = pos->m_lchild;
      if (p1->m_balance_factor == 0) {  // LL
        pos->m_lchild = p1->m_rchild;
        p1->m_rchild = pos;
        p1->m_balance_factor = -1;
        pos->m_balance_factor = 1;
        pos = p1;
        shorter = 0;
      } else if (p1->m_balance_factor == 1) { // RL
        pos->m_lchild = p1->m_rchild;
        p1->m_rchild = pos;
        pos->m_balance_factor = 0;
        p1->m_balance_factor = 0;
        pos = p1;
        shorter = 1;
      } else {  // LR
        p2 = p1->m_rchild;
        p1->m_rchild = p2->m_lchild;
        p2->m_lchild = p1;
        pos->m_lchild = p2->m_rchild;
        p2->m_rchild = pos;
        if (p2->m_balance_factor == 0) {
          pos->m_balance_factor = 0;
          p1->m_balance_factor = 0;
        } else if (p2->m_balance_factor == 1) {
          pos->m_balance_factor = -1;
          p1->m_balance_factor = 0;
        } else {
          pos->m_balance_factor = 0;
          p1->m_balance_factor = 1;
        }
        p2->m_balance_factor = 0;
        pos = p2;
        shorter = 1;
      }
    }
  }

};